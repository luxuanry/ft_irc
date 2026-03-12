#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

Server::Server(int port, std::string pass)
{
    _pass = pass;
    _initSocket(port);
}

Server::~Server()
{
    //close(m_socket);
	for (size_t i = 0; i < v_fds.size(); ++i)
        close(v_fds[i].fd);
    std::cout << "Server closed" << std::endl;
}

int& Server::getSocket() { return m_socket; }
std::vector<struct pollfd>& Server::getFds() { return v_fds; }
std::string Server::getPass() const { return _pass; }

void	Server::_initSocket(int port)
{
	//create socket 
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw "socket() error";
    fcntl (m_socket, F_SETFL, O_NONBLOCK);

    int opt = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw "setsockopt() error";
        
    //bind  socket
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if (bind(m_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
        throw "bind() error";

    //listen to connect
    if (listen(m_socket, 10) == -1)
        throw "listen() error";

    //add to the eventlist
    struct pollfd pfd;
    pfd.fd = m_socket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    v_fds.push_back(pfd);

    std::cout << "server is ready!" << std::endl;
}


void Server::_acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Accept the new connection, get client_fd
    int client_fd = accept(m_socket, (struct sockaddr*)&clientAddr, &clientLen);
    if (client_fd == -1)
        return;

    // Set non-blocking mode
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    // Add to poll watch list
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
    v_fds.push_back(pfd);

    std::cout << "New client fd=" << client_fd
              << " from " << inet_ntoa(clientAddr.sin_addr)
              << ":" << ntohs(clientAddr.sin_port) << std::endl;
}

void Server::run()
{
	while (true)
	{
		int poll_ret = poll(&v_fds[0], v_fds.size(), -1);
		if (poll_ret == -1)
			throw "poll() error";

		for (size_t i = 0; i < v_fds.size(); ++i)
		{
			if (v_fds[i].revents & POLLIN)
			{
				if (v_fds[i].fd == m_socket)
				{
					// Accept new client connection
					_acceptClient();
				}
				else
				{
					// 1. Receive data from the client
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_read = recv(v_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                    if (bytes_read <= 0)
                    {
                        // 2. Handle disconnection (recv returns 0 or -1)
                        std::cout << "Client " << v_fds[i].fd << " disconnected." << std::endl;
                        close(v_fds[i].fd);
                        _userManager.removeUser(v_fds[i].fd);
                        v_fds.erase(v_fds.begin() + i);
                        i--; // Adjust index after erase
                        continue;
                    }
                    else
                    {
                        // 3. Pass data to User class (Diagram: "storing input data to readbuffer")
                        _userManager.handleClientData(v_fds[i].fd, std::string(buffer));
                    }
				}
				
				// 4. Check for errors or client hang-ups (Diagram: "POLLHUP/POLLERR")
            	if (i < v_fds.size() && (v_fds[i].revents & (POLLHUP | POLLERR)))
            	{
                	std::cout << "Client " << v_fds[i].fd << " error/hup." << std::endl;
                	close(v_fds[i].fd);
                	_userManager.removeUser(v_fds[i].fd);
                	v_fds.erase(v_fds.begin() + i);
                	i--;
            	}
			}
			else if (v_fds[i].revents & POLLOUT)
			{
				// 1. Get the user's write buffer from the User class
				userInfo &user = _userManager.getUserInfo(v_fds[i].fd);

				// 2. If there is data to send, send it to the client
				if (!user.writeBuffer.empty())
				{
					int bytes_sent = send(v_fds[i].fd, user.writeBuffer.c_str(), user.writeBuffer.size(), 0);

					if (bytes_sent > 0)
					{
						// 3. Remove the sent data from the write buffer
						user.writeBuffer.erase(0, bytes_sent);
					}
					else
					{
						// Handle send error (optional)
						std::cout << "Error sending to client " << v_fds[i].fd << std::endl;
					}
				}
			}
		}
	}
}
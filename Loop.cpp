#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <csignal>

bool stop = false;

void sigint(int num)
{
    if (num == SIGINT || num == SIGQUIT)
        stop = true;

}

void handleClientPollIn(int fd, User &userManager, Channel &channelManager, std::vector<struct pollfd> &fds, size_t &index, std::string serverPass)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    // 1. Receive data
    int bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read <= 0) 
    {
        // Handle Disconnection
        std::cout << "Client " << fd << " disconnected." << std::endl;
        close(fd);
        userManager.removeUser(fd);
        fds.erase(fds.begin() + index);
        index--; // Adjust index because the vector size changed
    } 
    else 
    {
        // 2. Pass to User class for buffer management and command parsing
        userManager.handleClientData(fd, std::string(buffer), serverPass, channelManager);
    }
}

void handleClientPollOut(Server &irc, User &userManager, size_t &i)
{
    std::vector<struct pollfd> &fds = irc.getFds();
    
    int fd = fds[i].fd;
    struct userInfo &info = userManager.getUserInfo(fd);
    std::string &wBuf = info.writeBuffer;

    // If there is data to send, send it
	if (!wBuf.empty())
	{
		int byte_sent = send(fd, wBuf.c_str(), wBuf.size(), 0);
		if (byte_sent > 0)
		{
			wBuf.erase(0, byte_sent); // Remove sent data from buffer
			// If buffer is now empty, disable POLLOUT
        	if (wBuf.empty()) {
            	fds[i].events &= ~POLLOUT;
        }
		}
		else if (byte_sent == -1)
		{
			// Handle send error (e.g., client disconnected)
			std::cout << "Error sending to client " << fd << ". Disconnecting." << std::endl;
			close(fd);
			userManager.removeUser(fd);
			fds.erase(fds.begin() + i);
			i--; // Adjust index after erasing
		}
	}
    
	if (wBuf.empty() && info.status == -1)
	{
		std::cout << "Closing connection for FD " << fd << " (QUIT processed) " << std::endl;
		close(fd);
		userManager.removeUser(fd);
		fds.erase(fds.begin() + i);
		i--; // Adjust index after erasing
		return ;
	}

	if(wBuf.empty() && info.status != -1)
        fds[i].events &= ~POLLOUT; // Disable POLLOUT if there's nothing to write
}

void handleServerPollIn(Server &irc, User &userManager)
{
    try
    {
        std::pair<int, std::string> client = irc.acceptClient();
        userManager.addUserMapInt(client.first);
        userManager.setHostName(client.first, client.second);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
}


void startServerLoop(Server &irc)
{
    User userManager;
    Channel channelManager;
    signal(SIGINT, sigint);
    signal(SIGQUIT, sigint);
    
    while (!stop)
    {
        std::vector<struct pollfd> &fds = irc.getFds();
        int poll_ret = poll(&fds[0], fds.size(), -1);
        
        if (poll_ret == -1) break;

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == irc.getSocket())
                    handleServerPollIn(irc, userManager);
                else
                {
                    handleClientPollIn(fds[i].fd, userManager, channelManager, fds, i, irc.getPass());
                }
            }
            if (!userManager.getUserInfo(fds[i].fd).writeBuffer.empty() ||
						userManager.getUserInfo(fds[i].fd).status == -1)
                fds[i].events |= POLLOUT;
            // Handle POLLOUT and other events as needed
            if (fds[i].revents & POLLOUT)
            {
                handleClientPollOut(irc, userManager, i);
            }
        }
    }
}
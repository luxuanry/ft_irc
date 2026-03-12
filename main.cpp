/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 12:55:23 by suna              #+#    #+#             */
/*   Updated: 2026/03/12 13:45:46 by lcao             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "User.hpp"
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    int port;
    int m_socket = -1;
    std::string pass;
    std::vector<struct pollfd> v_fds;

    User userManager;
    //Server myServer(port, pass);

    try
    {
        if (argc != 3)
            throw "argument error";
        port = atoi(argv[1]);
        pass = (argv[2]);
        if (!(1024 <= port && port <= 49151))
            throw "port number error";
        
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

        while(true)
        {
            int poll_ret = poll(&v_fds[0], v_fds.size(), -1);
            if(poll_ret == -1)
                throw "poll() error";
            for(size_t i = 0; i < v_fds.size(); i++)
            {
                if(v_fds[i].revents & POLLIN )
                {
                    if(v_fds[i].fd == m_socket)
                    {
                        //accept new connection
                        struct sockaddr_in clientAddr;
                        socklen_t clientAddrLen = sizeof(clientAddr);
                        int client_fd = accept(m_socket, (struct sockaddr *)&clientAddr, &clientAddrLen);
                        if(client_fd == -1)
                        {
                            std::cerr << "Error: failed to accept new connection, ignoring..." << std::endl;
                            continue;
                        }
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        struct pollfd client_pfd;
                        client_pfd.fd = client_fd;
                        client_pfd.events = POLLIN;
                        client_pfd.revents = 0;
                        v_fds.push_back(client_pfd);
                        
                        userManager.addUserListInt(client_fd);
                        std::cout << "new client connected: " << client_fd << std::endl;
                    }
                    else
                    {
                        //handle client data
                        char buffer[1024];
                        memset(buffer, 0, sizeof(buffer));

                        int bytes_read = recv(v_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                        if(bytes_read <= 0)
                        {
                            //client disconnected
                            std::cout << "client disconnected: " << v_fds[i].fd << std::endl;
                            userManager.removeUser(v_fds[i].fd);
                            close(v_fds[i].fd);
                            v_fds.erase(v_fds.begin() + i);
                            //adjust index after erasing
                            i--;
                        }
                        else
                        {
                            userManager.getUserInfo(v_fds[i].fd).readBuffer += buffer;
                            std::cout << "received from client " << v_fds[i].fd << ": " << buffer << std::endl;
                            v_fds[i].events |= POLLOUT;
                        }
                    }
                }
                if(v_fds[i].revents & POLLOUT)
                {
                    const char * response = "Message received\r\n";
                    send(v_fds[i].fd, response, strlen(response), 0);
                    v_fds[i].events &= ~POLLOUT;
                }
                if(v_fds[i].revents & (POLLHUP | POLLERR))
                {
                    std::cout << "Client error/hup: " << v_fds[i].fd << std::endl;
                    userManager.removeUser(v_fds[i].fd);
                    
                    close(v_fds[i].fd);
                    v_fds.erase(v_fds.begin() + i);
                    i--;
                }
            }
        }
    }

        std::cout << "test is over" << std::endl;


        close(m_socket);
        return 0;
    }
    catch(const char * str)
    {
        std::cerr << str << std::endl;
        if(m_socket != -1){
            close(m_socket);
        }
        return 1;
    }


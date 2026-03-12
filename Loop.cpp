#include "Server.hpp"
#include "User.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

void serverPollin(Server &irc, User &userManager)
{
    try
    {
        std::pair<int, std::string> client = irc._acceptClient();
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
    
    while (true)
    {
        std::vector<struct pollfd> &fds = irc.getFds();
        int poll_ret = poll(&fds[0], fds.size(), -1);
        
        if (poll_ret == -1) break;

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == irc.getSocket())
                    serverPollin(irc, userManager);
                else
                {
                    char buf[1024] = {0};
                    int bytes = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);
                    
                    if (bytes <= 0)
                    {
                        int fd = fds[i].fd;
                        close(fd);
                        userManager.removeUser(fd);
                        fds.erase(fds.begin() + i);
                        i--;
                    }
                    else
                    {
                        // Direct access to User class logic
                        userManager.handleClientData(fds[i].fd, std::string(buf));
                    }
                }
            }
            // Handle POLLOUT and other events as needed
        }
    }
}
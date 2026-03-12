#include "Server.hpp"
#include "User.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

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
                {
                    irc._acceptClient();
                    // Link the new FD to the user logic
                    userManager.addUserListInt(fds.back().fd);
                }
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
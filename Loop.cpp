#include "Server.hpp"
#include "User.hpp"
#include "HandleClientPollout.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

void handleClientPollIn(int fd, User &userManager, std::vector<struct pollfd> &fds, size_t &index)
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
        userManager.handleClientData(fd, std::string(buffer));
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
                {
                    irc._acceptClient();
                    // Link the new FD to the user logic
                    userManager.addUserListInt(fds.back().fd);
                }
                else
                {
                    handleClientPollIn(fds[i].fd, userManager, fds, i);
                }
            }
            // Handle POLLOUT and other events as needed
            if (fds[i].revents & POLLOUT)
            {
                handleClientPollOut(irc, userManager, i);
            }
        }
    }
}
#include "Server.hpp"
#include "User.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

void handleClientPollOut(Server &irc, User &userManager, size_t &i)
{
    std::vector<struct pollfd> &fds = irc.getFds();
    
    int fd = fds[i].fd;
    std::string &wBuf = userManager.getUserInfo(fd).writeBuffer;

    if(wBuf.empty())
    {
        fds[i].events &= ~POLLOUT; // Disable POLLOUT if there's nothing to write
        return;
    }
    // Attempt to send data
    int bytes_sent = send(fd, wBuf.c_str(), wBuf.size(), 0);

    // Check if send was successful
    if (bytes_sent > 0)
    {
        wBuf.erase(0, bytes_sent); // Remove sent data from buffer
        
        // If buffer is now empty, disable POLLOUT
        if (wBuf.empty()) {
            fds[i].events &= ~POLLOUT;
        }
    }
    else if(bytes_sent == -1){
        // Handle send error (e.g., client disconnected)
        close(fd);
        userManager.removeUser(fd);
        fds.erase(fds.begin() + i);
        i--; // Adjust index after erasing
    }

}
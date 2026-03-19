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

void cleanupUserChannels(int fd, User &userManager, Channel &channelManager)
{
    struct userInfo &info = userManager.getUserInfo(fd);
    std::set<std::string>::iterator it;
    for (it = info.channelList.begin(); it != info.channelList.end(); ++it)
        channelManager.removeUserFromChannel(*it, fd);
    channelManager.removeFromAllInviteLists(fd);
}

bool handleClientPollIn(int fd, User &userManager, Channel &channelManager, std::vector<struct pollfd> &fds, size_t &index, std::string serverPass)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    // 1. Receive data
    int bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read == -1)
    {
        // poll() already confirmed POLLIN, so any recv error is a real error
        std::cout << "Client " << fd << " recv error." << std::endl;
        cleanupUserChannels(fd, userManager, channelManager);
        close(fd);
        userManager.removeUser(fd);
        fds.erase(fds.begin() + index);
        index--;
        return true;
    }
    if (bytes_read == 0)
    {
        // Client closed connection
        std::cout << "Client " << fd << " disconnected." << std::endl;
        cleanupUserChannels(fd, userManager, channelManager);
        close(fd);
        userManager.removeUser(fd);
        fds.erase(fds.begin() + index);
        index--;
        return true;
    }

    // 2. Check readBuffer size limit (4096 bytes) - flood protection
    std::string &readBuf = userManager.getUserInfo(fd).readBuffer;
    if (readBuf.size() + bytes_read > 512)
    {
        std::cout << "Client " << fd << " exceeded buffer limit. Disconnecting." << std::endl;
        std::string errMsg = "ERROR :Input buffer overflow\r\n";
        send(fd, errMsg.c_str(), errMsg.size(), 0);
        cleanupUserChannels(fd, userManager, channelManager);
        close(fd);
        userManager.removeUser(fd);
        fds.erase(fds.begin() + index);
        index--;
        return true;
    }

    // 3. Pass to User class for buffer management and command parsing
    userManager.handleClientData(fd, std::string(buffer), serverPass, channelManager);
    return false;
}

void handleClientPollOut(Server &irc, User &userManager, Channel &channelManager, size_t &i)
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
			cleanupUserChannels(fd, userManager, channelManager);
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
        
        if (poll_ret == -1)
        {
            if (errno == EINTR)
                continue;
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i)
        {
            // Skip server socket for client handling
            if (fds[i].fd == irc.getSocket())
            {
                if (fds[i].revents & POLLIN)
                    handleServerPollIn(irc, userManager);
                continue;
            }

            // Handle error/hangup - disconnect the client
            if (fds[i].revents & (POLLERR | POLLHUP))
            {
                std::cout << "Client " << fds[i].fd << " error/hangup." << std::endl;
                cleanupUserChannels(fds[i].fd, userManager, channelManager);
                close(fds[i].fd);
                userManager.removeUser(fds[i].fd);
                fds.erase(fds.begin() + i);
                i--;
                continue;
            }

            if (fds[i].revents & POLLIN)
            {
                if (handleClientPollIn(fds[i].fd, userManager, channelManager, fds, i, irc.getPass()))
                    continue; // Client was removed, skip to next
            }

            if (!userManager.getUserInfo(fds[i].fd).writeBuffer.empty() ||
						userManager.getUserInfo(fds[i].fd).status == -1)
                fds[i].events |= POLLOUT;

            if (fds[i].revents & POLLOUT)
            {
                handleClientPollOut(irc, userManager, channelManager, i);
            }
        }
    }

    // Server shutting down - notify all clients
    std::vector<struct pollfd> &fds = irc.getFds();
    std::string shutdownMsg = "ERROR :Server shutting down\r\n";
    for (size_t i = 0; i < fds.size(); ++i)
    {
        if (fds[i].fd != irc.getSocket())
            send(fds[i].fd, shutdownMsg.c_str(), shutdownMsg.size(), 0);
    }
}
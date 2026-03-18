#include "../commands.hpp"

#include "../commands.hpp"

void quit(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);
    
    // 1. Capture the full reason (joining all words after QUIT)
    std::string reason = "Client Quit";
    if (cmd.size() > 1) {
        reason = cmd[1];
        for (size_t i = 2; i < cmd.size(); i++)
            reason += " " + cmd[i];
        if (!reason.empty() && reason[0] == ':') 
            reason.erase(0, 1);
    }

    // 2. If the user was logged in, notify all their channels
    if (user.isLogin(fd)) {
        std::string quitNotice = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName + " QUIT :" + reason + "\r\n";
        
        // We create a copy of the list to avoid iterator invalidation
        std::set<std::string> channelsJoined = info.channelList; 
        for (std::set<std::string>::iterator it = channelsJoined.begin(); it != channelsJoined.end(); ++it) {
            std::set<int> &users = channels.getUsers(*it);
            for (std::set<int>::iterator uIt = users.begin(); uIt != users.end(); ++uIt) {
                if (*uIt != fd) // Don't send back to the quitting user
                    user.setWrtieBuffer(*uIt, quitNotice);
            }
            channels.removeUserFromChannel(*it, fd);
        }
        info.channelList.clear();
    }

    // 3. Send the final ERROR message
    user.setWrtieBuffer(fd, "ERROR :Closing Link: " + info.hostName + " (" + reason + ")\r\n");

    // 4. Set status to -1 so Loop.cpp knows to close the socket
    info.status = -1;
    std::cout << "FD " << fd << " (Nick: " << info.nickName << ") marked for removal." << std::endl;
}
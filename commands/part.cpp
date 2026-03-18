#include "../commands.hpp"

#include "../commands.hpp"

void part(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);
    
    if (info.status < 1) {
        user.setWrtieBuffer(fd, "451 :You have not registered\r\n");
        return;
    }
    if (cmd.size() < 2) {
        user.setWrtieBuffer(fd, "461 PART :Not enough parameters\r\n");
        return;
    }

    std::string channelName = cmd[1];
    if (!channels.isExist(channelName)) {
        user.setWrtieBuffer(fd, "403 " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channels.isUserInChannel(channelName, fd)) {
        user.setWrtieBuffer(fd, "442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // Capture full reason: join everything from cmd[2] onwards
    std::string reason = "";
    if (cmd.size() > 2) {
        reason = cmd[2];
        for (size_t i = 3; i < cmd.size(); i++)
            reason += " " + cmd[i];
        if (reason[0] == ':') reason.erase(0, 1);
    }

    std::string msg = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName + " PART " + channelName;
    if (!reason.empty()) msg += " :" + reason;
    msg += "\r\n";
    
    // Broadcast to channel members
    std::set<int> &users = channels.getUsers(channelName);
    for (std::set<int>::iterator it = users.begin(); it != users.end(); ++it)
        user.setWrtieBuffer(*it, msg);

    channels.removeUserFromChannel(channelName, fd);
    info.channelList.erase(channelName);
    std::cout << "User " << info.nickName << " has left channel " << channelName << std::endl;
}
#include "../Channel.hpp"
#include "../commands.hpp"

void kickCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd)
{
    std::string execNick = userObj.getNickName(fd); 
    if (cmds.size() < 3) {
        std::string errMsg = ":server 461 " + execNick + " KICK :Not enough parameters\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }

    std::string chanName = cmds[1];
    std::string targetNick = cmds[2];
  
    std::string reason = (cmds.size() > 3) ? cmds[3] : "No reason specified";

    std::string nick = userObj.getNickName(fd);

    if(!chanObj.isExist(chanName)){
        std::string errMsg = ":server 403 " + nick + " " + chanName + " :No such channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }
    
    if(!chanObj.isUserInChannel(chanName, fd)){
        std::string errMsg = ":server 442 " + nick + " " + chanName + " :You're not on that channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }

    int targetFd = userObj.getFdByNick(targetNick);
    if (targetFd == -1) {
        std::string errMsg = ":server 401 " + execNick + " " + targetNick + " :No such nick/channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }

    struct channelInfo &info = chanObj.getChannelInfo(chanName);

    if (info.operators.find(fd) == info.operators.end()) {
        std::string errMsg = ":server 482 " + execNick + " " + chanName + " :You're not channel operator\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }

    if (info.users.find(targetFd) == info.users.end()) {
        std::string errMsg = ":server 441 " + execNick + " " + targetNick + " " + chanName + " :They aren't on that channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }

    std::string kickMsg = ":" + execNick + " KICK " + chanName + " " + targetNick + reason + "\r\n";

    for (std::set<int>::iterator it = info.users.begin(); it != info.users.end(); ++it) {
        userObj.setWrtieBuffer(*it, kickMsg);
    }

    info.users.erase(targetFd);
    if (info.operators.find(targetFd) != info.operators.end()) {
        info.operators.erase(targetFd);
    }
}
#include "../Channel.hpp"
#include "../commands.hpp"
#include <iostream>

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

    if(!chanObj.isExist(chanName)){
        std::string errMsg = ":server 403 " + execNick + " " + chanName + " :No such channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }
    
    if(!chanObj.isUserInChannel(chanName, fd)){
        std::string errMsg = ":server 442 " + execNick + " " + chanName + " :You're not on that channel\r\n";
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

    // fix: when there is blank space
    std::string reason = "No reason specified";
    if (cmds.size() > 3) {
        reason = "";
        for (size_t i = 3; i < cmds.size(); ++i) {
            reason += cmds[i];
            if (i != cmds.size() - 1) reason += " ";
        }
        if (!reason.empty() && reason[0] == ':') {
            reason = reason.substr(1);
        }
    }


    std::string kickMsg = ":" + execNick + " KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";
    // 1.broadcast before kicking
    for (std::set<int>::iterator it = info.users.begin(); it != info.users.end(); ++it) {
        userObj.setWrtieBuffer(*it, kickMsg);
    }

    // 2. check if the only member in the channel is operator
    bool willAutoPromote = false;
    if (info.operators.size() == 1 && info.operators.find(targetFd) != info.operators.end() && info.users.size() > 1) {
        willAutoPromote = true; 
    }

    chanObj.removeUserFromChannel(chanName, targetFd);
    userObj.getUserInfo(targetFd).channelList.erase(chanName);

    if (willAutoPromote) {
        if (chanObj.isExist(chanName)) {
            struct channelInfo &updatedInfo = chanObj.getChannelInfo(chanName);
            int newOpFd = *(updatedInfo.operators.begin()); 
            
            std::string newOpNick = userObj.getNickName(newOpFd);
            std::string opMsg = ":server MODE " + chanName + " +o " + newOpNick + "\r\n";

            for (std::set<int>::iterator it = updatedInfo.users.begin(); it != updatedInfo.users.end(); ++it) {
                userObj.setWrtieBuffer(*it, opMsg);
            }
            std::cout << "[DEBUG] " << newOpNick << " auto-promoted to operator in " << chanName << std::endl;
        }
    }
}
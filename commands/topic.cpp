#include "../Channel.hpp"
#include "../commands.hpp"

void topicCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd)
{
    std::string nick = userObj.getNickName(fd);

    if (cmds.size() < 2) {
        std::string errMsg = ":server 461 " + nick + " TOPIC :Not enough parameters\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }
    std::string chanName = cmds[1];

    if(!chanObj.isChannelExist(chanName)){
        std::string errMsg = ":server 403 " + nick + " " + chanName + " :No such channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }
    
    if(!chanObj.isUserInChannel(chanName, fd)){
        std::string errMsg = ":server 442 " + nick + " " + chanName + " :You're not on that channel\r\n";
        userObj.setWrtieBuffer(fd, errMsg);
        return;
    }

    struct channelInfo &info = chanObj.getChannelInfo(chanName);
    if (cmds.size() == 2) {
            if (info.topic.empty()) {
                std::string msg = ":server 331 " + nick + " " + chanName + " :No topic is set\r\n";
                userObj.setWrtieBuffer(fd, msg);
                return ;
            }
            else {
                std::string msg = ":server 332 " + nick + " " + chanName + " :" + info.topic + "\r\n";
                userObj.setWrtieBuffer(fd, msg);
                return ;
            }
        }
        else {
            if (info.topicRestriced && info.operators.find(fd) == info.operators.end()) {
                std::string errMsg = ":server 482 " + nick + " " + chanName + " :You're not channel operator\r\n";
                userObj.setWrtieBuffer(fd, errMsg);
                return;
            }
        }
        info.topic = cmds[2];

        std::string broadcastMsg = ":" + nick + " TOPIC " + chanName + " :" + info.topic + "\r\n";
    }
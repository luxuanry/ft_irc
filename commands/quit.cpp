#include "../commands.hpp"

void quit(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);

    // 1. Get the reason
    std::string reason = (cmd.size() > 1) ? cmd[1] : "Client Quit";
    if (reason[0] == ':') reason.erase(0, 1);

    // 2. PART from all channels (sends PART messages, not QUIT)
    if (user.isLogin(fd))
    {
        std::string prefix = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName;
        std::string quitMsg = prefix + " QUIT :" + reason + "\r\n";
        std::set<int> notified;
        std::set<std::string> channelsCopy = info.channelList;
        std::set<std::string>::iterator it;
        for (it = channelsCopy.begin(); it != channelsCopy.end(); ++it)
        {
            std::string channelName = *it;

            std::set<int> &channelUsers = channels.getUsers(channelName);
            for (std::set<int>::iterator uIt = channelUsers.begin(); uIt != channelUsers.end(); ++uIt)
            {
                if (*uIt != fd && notified.find(*uIt) == notified.end())
                {
                    user.getUserInfo(*uIt).writeBuffer += quitMsg;
                    notified.insert(*uIt);
                }
            }
            channels.removeUserFromChannel(channelName, fd);
        }
        info.channelList.clear();
    }

    // 3. Prepare the final ERROR message for the quitting client
    std::string msg = "Closing: " + info.hostName + " (" + reason + ")\r\n";
    info.writeBuffer += msg;

    // 4. Mark for deletion in the next POLL cycle
    info.status = -1;

    std::cout << "FD " << fd << " (Nick: " << info.nickName << ") has quit." << std::endl;
}

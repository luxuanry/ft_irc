#include "../commands.hpp"

void quit(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);
    std::string reason = (cmd.size() > 1) ? cmd[1] : "Client Quit";
    if (!reason.empty() && reason[0] == ':') reason.erase(0, 1);

    if (user.isLogin(fd))
    {
        std::string quitNotice = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName + " QUIT :" + reason + "\r\n";
        
        // 1. Collect UNIQUE targets (people who share any channel with the quitter)
        std::set<int> targets;
        for (std::set<std::string>::iterator it = info.channelList.begin(); it != info.channelList.end(); ++it)
        {
            std::set<int> &members = channels.getUsers(*it);
            for (std::set<int>::iterator uIt = members.begin(); uIt != members.end(); ++uIt)
            {
                if (*uIt != fd)
                    targets.insert(*uIt);
            }
        }

        // 2. Send the message to each unique target ONCE
        for (std::set<int>::iterator tIt = targets.begin(); tIt != targets.end(); ++tIt)
        {
            user.getUserInfo(*tIt).writeBuffer += quitNotice;
            // IMPORTANT: Ensure POLLOUT is enabled for these targets in the main loop!
        }

        // 3. Clean up the quitter's channel presence
        for (std::set<std::string>::iterator it = info.channelList.begin(); it != info.channelList.end(); ++it)
            channels.removeUserFromChannel(*it, fd);
        
        info.channelList.clear();
    }

    // 4. Final farewell to the quitter
    user.getUserInfo(fd).writeBuffer += "ERROR :Closing Link: " + info.hostName + " (" + reason + ")\r\n";
    info.status = -1;
    std::cout << "FD " << fd << " (Nick: " << info.nickName << ") marked for removal." << std::endl;
}

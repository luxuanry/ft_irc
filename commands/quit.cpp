#include "../commands.hpp"

void quit(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);
    
    // 1. Get the reason
    std::string reason = (cmd.size() > 1) ? cmd[1] : "Client Quit";
    if (reason[0] == ':') reason.erase(0, 1);

    // 2. Notify all channels this user was in
    if (user.isLogin(fd))
    {
        std::string quitNotice = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName + " QUIT :" + reason + "\r\n";
        std::set<std::string>::iterator it = info.channelList.begin();
        for (; it != info.channelList.end(); ++it)
        {
            std::string channelName = *it; 
            std::set<int> &channelUsers = channels.getUsers(channelName);
            for (std::set<int>::iterator uIt = channelUsers.begin(); uIt != channelUsers.end(); ++uIt)
            {
                if (*uIt != fd) // Don't send it to the quitting user themselves
                    user.getUserInfo(*uIt).writeBuffer += quitNotice;
            }
            channels.removeUserFromChannel(channelName, fd);
        }
        
        info.channelList.clear();
    }

    // 3. Prepare the final ERROR message for the quitting client
    std::string msg = "ERROR :Closing Link: " + info.hostName + " (" + reason + ")\r\n";
    info.writeBuffer += msg;

    // 4. Mark for deletion in the next POLL cycle
    info.status = -1;

    std::cout << "FD " << fd << " (Nick: " << info.nickName << ") has quit." << std::endl;
}
#include "../commands.hpp"

void part(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
	struct userInfo &info = user.getUserInfo(fd);
	std::string nick = user.getNickName(fd);
	if (nick.empty())
		nick = "*";

<<<<<<< HEAD
	// 1. Check if the user is logged in
	if (info.status < 1)
	{
		info.writeBuffer += ":server 451 " + nick + " :You have not registered\r\n";
		return;
	}

	// 2. Check if channel name is provided
	if (cmd.size() < 2)
	{
		info.writeBuffer += ":server 461 " + nick + " PART :Not enough parameters\r\n";
		return;
	}

	std::string channelName = cmd[1];

	// 3. Check if the channel exists
	if (!channels.isExist(channelName))
	{
		info.writeBuffer += ":server 403 " + nick + " " + channelName + " :No such channel\r\n";
		return;
	}

	// 4. Check if the user is actually in the channel
	if (info.channelList.find(channelName) == info.channelList.end())
	{
		info.writeBuffer += ":server 442 " + nick + " " + channelName + " :You're not on that channel\r\n";
		return;
	}

	// 5. Build PART message before removing user
	std::string reason = (cmd.size() > 2) ? cmd[2] : "";
	std::string msg = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName + " PART " + channelName;
	if (!reason.empty()) msg += " " + reason;
	msg += "\r\n";

	// 6. Send PART message to all channel members (including the parting user)
	std::set<int> &channelUsers = channels.getUsers(channelName);
	for (std::set<int>::iterator it = channelUsers.begin(); it != channelUsers.end(); ++it)
		user.setWrtieBuffer(*it, msg);

	// 7. Remove user from channel's user list and user's channel list
	channels.removeUserFromChannel(channelName, fd);
	info.channelList.erase(channelName);

	std::cout << "User " << info.nickName << " has left channel " << channelName << std::endl;
}
=======
	// 4. Check if the user is actually in the channel
	if (info.channelList.find(channelName) == info.channelList.end())
	{
		info.writeBuffer += "442 " + channelName + " :You're not on that channel\r\n";
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
>>>>>>> 779a71e3ce258659437a6b09e6c080bc370d168e

#include "../commands.hpp"

void part(User &user, Channel &channels, std::vector<std::string> cmd, int fd)
{
	struct userInfo &info = user.getUserInfo(fd);
	
	// 1. Check if the user is logged in
	if (info.status < 1)
	{
		info.writeBuffer += "451 :You have not registered\r\n";
		return;
	}

	// 2. Check if channel name is provided
	if (cmd.size() < 2)
	{
		info.writeBuffer += "461 PART :Not enough parameters\r\n";
		return;
	}

	std::string channelName = cmd[1];
	
	// 3. Check if the channel exists
	if (!channels.isExist(channelName))
	{
		info.writeBuffer += "403 " + channelName + " :No such channel\r\n";
		return;
	}

	// 4. Check if the user is actually in the channel
	channelInfo &chanInfo = channels.getChannelInfo(channelName);
	if (info.channelList.find(channelName) == info.channelList.end())
	{
		info.writeBuffer += "442 " + channelName + " :You're not on that channel\r\n";
		return;
	}

	// 5. Remove user from channel's user list and user's channel list
	chanInfo.users.erase(fd);
	info.channelList.erase(channelName);

	// 6. Broadcast PART message to remaining channel members
	std::string reason = (cmd.size() > 2) ? cmd[2] : "";
    std::string msg = ":" + info.nickName + "!" + info.loginName + "@" + info.hostName + " PART " + channelName;
    if (!reason.empty()) msg += " " + reason;
    msg += "\r\n";
    
    user.setBroadCastMsg(msg);

	std::cout << "User " << info.nickName << " has left channel " << channelName << std::endl;

}
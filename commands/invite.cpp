
#include "../commands.hpp"

void invite(User &user, Channel &channel, std::vector<std::string> cmd, int fd)
{
	struct userInfo &info = user.getUserInfo(fd);
	std::string nick = user.getNickName(fd);

	// ERR_NEEDMOREPARAMS (461)
	if (cmd.size() < 3)
	{
		info.writeBuffer += ":server 461 " + nick + " INVITE :Not enough parameters\r\n";
		return;
	}

	std::string targetNick = cmd[1];
	std::string channelName = cmd[2];

	// ERR_NOSUCHCHANNEL (403)
	if (!channel.isExist(channelName))
	{
		info.writeBuffer += ":server 403 " + nick + " " + channelName + " :No such channel\r\n";
		return;
	}

	// ERR_NOTONCHANNEL (442) - inviter must be on channel
	if (!channel.isUserInChannel(channelName, fd))
	{
		info.writeBuffer += ":server 442 " + nick + " " + channelName + " :You're not on that channel\r\n";
		return;
	}

	// ERR_CHANOPRIVSNEEDED (482) - if invite-only, must be operator
	channelInfo &chInfo = channel.getChannelInfo(channelName);
	if (chInfo.inviteOnly && !channel.isOperator(channelName, fd))
	{
		info.writeBuffer += ":server 482 " + nick + " " + channelName + " :You're not channel operator\r\n";
		return;
	}

	// ERR_NOSUCHNICK (401) - target must exist
	if (!user.isExist(targetNick))
	{
		info.writeBuffer += ":server 401 " + nick + " " + targetNick + " :No such nick/channel\r\n";
		return;
	}

	int targetFd = user.getFdByNick(targetNick);

	// ERR_USERONCHANNEL (443) - target already on channel
	if (channel.isUserInChannel(channelName, targetFd))
	{
		info.writeBuffer += ":server 443 " + nick + " " + targetNick + " " + channelName + " :is already on channel\r\n";
		return;
	}

	// Add target to invite list
	channel.addToInviteList(channelName, targetFd);

	// RPL_INVITING (341) - confirm to inviter
	info.writeBuffer += ":server 341 " + nick + " " + targetNick + " " + channelName + "\r\n";

	// Send INVITE message to target
	std::string prefix = ":" + nick + "!" + user.getLoginName(fd) + "@" + user.getHostName(fd);
	user.setWrtieBuffer(targetFd, prefix + " INVITE " + targetNick + " " + channelName + "\r\n");
}

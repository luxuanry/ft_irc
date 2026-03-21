
#include "../commands.hpp"

static std::string makePrefix(User &user, int fd)
{
	return ":" + user.getNickName(fd) + "!" + user.getLoginName(fd) + "@" + user.getHostName(fd);
}

void join(User &user, Channel &channel, std::vector<std::string> cmd, int fd)
{
	struct userInfo &info = user.getUserInfo(fd);

	// ERR_NEEDMOREPARAMS (461)
	if (cmd.size() < 2)
	{
		info.writeBuffer += ":server 461 " + user.getNickName(fd) + " JOIN :Not enough parameters\r\n";
		return;
	}

	std::string channelName = cmd[1];

	// Channel name must start with #
	if (channelName.empty() || channelName[0] != '#')
	{
		info.writeBuffer += ":server 403 " + user.getNickName(fd) + " " + channelName + " :No such channel\r\n";
		return;
	}

	// Already in channel
	if (channel.isUserInChannel(channelName, fd))
		return;

	// Create channel if it doesn't exist
	if (!channel.isExist(channelName))
	{
		channel.addChannel(channelName);
		// First user becomes operator
		channel.addOperator(channelName, fd);
	}
	else
	{
		channelInfo &chInfo = channel.getChannelInfo(channelName);

		// ERR_INVITEONLYCHAN (473)
		if (chInfo.inviteOnly && !channel.isInvited(channelName, fd))
		{
			info.writeBuffer += ":server 473 " + user.getNickName(fd) + " " + channelName + " :Cannot join channel (+i)\r\n";
			return;
		}

		// ERR_CHANNELISFULL (471)
		if (chInfo.hasLimit && (int)chInfo.users.size() >= chInfo.limit)
		{
			info.writeBuffer += ":server 471 " + user.getNickName(fd) + " " + channelName + " :Cannot join channel (+l)\r\n";
			return;
		}

		// ERR_BADCHANNELKEY (475)
		if (chInfo.hasKey)
		{
			std::string providedKey = cmd.size() > 2 ? cmd[2] : "";
			if (providedKey != chInfo.key)
			{
				info.writeBuffer += ":server 475 " + user.getNickName(fd) + " " + channelName + " :Cannot join channel (+k)\r\n";
				return;
			}
		}
	}

	// Add user to channel
	channel.addUserToChannel(channelName, fd);
	info.channelList.insert(channelName);

	// Remove from invite list if was invited
	channel.getChannelInfo(channelName).inviteList.erase(fd);

	// Send JOIN message to all users in the channel
	std::string prefix = makePrefix(user, fd);
	std::string joinMsg = prefix + " JOIN " + channelName + "\r\n";

	std::set<int> &users = channel.getUsers(channelName);
	std::set<int>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
		user.setWrtieBuffer(*it, joinMsg);

	// RPL_TOPIC (332) - send topic if set
	channelInfo &chInfo = channel.getChannelInfo(channelName);
	if (!chInfo.topic.empty())
		info.writeBuffer += ":server 332 " + user.getNickName(fd) + " " + channelName + " TOPIC " + chInfo.topic + "\r\n";

	// RPL_NAMREPLY (353) - send list of users in channel
	std::string nameList;
	for (it = users.begin(); it != users.end(); ++it)
	{
		if (!nameList.empty())
			nameList += " ";
		if (channel.isOperator(channelName, *it))
			nameList += "@";
		nameList += user.getNickName(*it);
	}
	info.writeBuffer += ":server 353 " + user.getNickName(fd) + " = " + channelName + " :" + nameList + "\r\n";

	// RPL_ENDOFNAMES (366)
	info.writeBuffer += ":server 366 " + user.getNickName(fd) + " " + channelName + " :End of /NAMES list\r\n";
}

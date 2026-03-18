
#include "../commands.hpp"

static std::string extractMessage(std::vector<std::string> &cmd)
{
	if (cmd.size() < 3)
		return "";
	std::string msg = cmd[2];
	for (size_t i = 3; i < cmd.size(); i++)
		msg += " " + cmd[i];
	// Remove leading ':' if present
	if (!msg.empty() && msg[0] == ':')
		msg = msg.substr(1);
	return msg;
}

void privmsg(User &user, Channel &channel, std::vector<std::string> cmd, int fd)
{
	struct userInfo &info = user.getUserInfo(fd);
	std::string nick = user.getNickName(fd);

	// ERR_NORECIPIENT (411)
	if (cmd.size() < 2)
	{
		info.writeBuffer += ":server 411 " + nick + " :No recipient given (PRIVMSG)\r\n";
		return;
	}

	// ERR_NOTEXTTOSEND (412)
	if (cmd.size() < 3)
	{
		info.writeBuffer += ":server 412 " + nick + " :No text to send\r\n";
		return;
	}

	std::string target = cmd[1];
	std::string message = extractMessage(cmd);
	std::string prefix = ":" + nick + "!" + user.getLoginName(fd) + "@" + user.getHostName(fd);

	// Channel message
	if (!target.empty() && target[0] == '#')
	{
		// ERR_NOSUCHCHANNEL (403)
		if (!channel.isExist(target))
		{
			info.writeBuffer += ":server 403 " + nick + " " + target + " :No such channel\r\n";
			return;
		}

		// ERR_CANNOTSENDTOCHAN (404) - must be in channel
		if (!channel.isUserInChannel(target, fd))
		{
			info.writeBuffer += ":server 404 " + nick + " " + target + " :Cannot send to channel\r\n";
			return;
		}

		// Send to all users in channel except sender
		std::string privMsg = prefix + " PRIVMSG " + target + " :" + message + "\r\n";
		std::set<int> &users = channel.getUsers(target);
		std::set<int>::iterator it;
		for (it = users.begin(); it != users.end(); ++it)
		{
			if (*it != fd)
				user.setWrtieBuffer(*it, privMsg);
		}
	}
	// Private message to user
	else
	{
		// ERR_NOSUCHNICK (401)
		if (!user.isExist(target))
		{
			info.writeBuffer += ":server 401 " + nick + " " + target + " :No such nick/channel\r\n";
			return;
		}

		int targetFd = user.getFdByNick(target);
		std::string privMsg = prefix + " PRIVMSG " + target + " :" + message + "\r\n";
		user.setWrtieBuffer(targetFd, privMsg);
	}
}

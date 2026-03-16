#include "../commands.hpp"

void pass(User &user, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);

    // Check if user is already registered
    if (info.status >= 1)
    {
        info.writeBuffer += "462 :Unauthorized command (already registered)\r\n";
        return;
    }

    // Check parameter count (cmds[0] is "PASS", cmds[1] is the password)
    if (cmd.size() < 2)
    {
        info.writeBuffer += "461 PASS :Not enough parameters\r\n";
        return;
    }
	
    info.password = cmd[1];
}
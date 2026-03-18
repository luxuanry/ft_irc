#include "../commands.hpp"

void pass(User &user, std::vector<std::string> cmd, int fd, const std::string serverPassword)
{
    struct userInfo &info = user.getUserInfo(fd);

    // 1. Check if already registered (Error 462)
    if (info.status >= 1)
    {
        info.writeBuffer += "462 :Unauthorized command (already registered)\r\n";
        return;
    }

    // 2. Check if password was provided (Error 461)
    if (cmd.size() < 2)
    {
        info.writeBuffer += "461 PASS :Not enough parameters\r\n";
        return;
    }

    // 3. THE COMPARISON
    if (cmd[1] != serverPassword)
    {
        info.writeBuffer += "464 :Password incorrect\r\n";
        return;
    }

    // 4. If they match, save it to the user's info
    info.password = cmd[1];
    std::cout << "FD " << fd << " authenticated with correct password." << std::endl;
}
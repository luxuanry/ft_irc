#include "../commands.hpp"
#include <iostream>

void userCmd(User &userObj, std::vector<std::string> cmds, int fd)
{
    struct userInfo &info = userObj.getUserInfo(fd);

    if (cmds.size() < 5)
    {
        std::string errorMsg = ":server 461 " + cmds[0] + " :Not enough parameters\r\n";
        info.writeBuffer += errorMsg; 
        return;
    }
    info.loginName = cmds[1];
}
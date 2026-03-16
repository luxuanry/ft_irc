#include "../commands.hpp"
#include <iostream>

void userCmd(User &userObj, std::vector<std::string> cmds, int fd)
{
    //get
    struct userInfo &info = userObj.getUserInfo(fd);

    if (cmds.size() < 5)
    {
        std::string errorMsg = ":server 461 " + cmds[0] + " :Not enough parameters\r\n";
        info.writeBuffer += errorMsg; 
        return;
    }

    info.loginName = cmds[1];
    info.hostName = cmds[2];

    if (userObj.isLogin(fd))
    {
        std::string successMsg = ":server 001 " + info.nickName + " :Welcome to the IRC server\r\n";
        info.writeBuffer += successMsg;
        std::cout << "User " << info.nickName << " logged in successfully." << std::endl;
    }
}
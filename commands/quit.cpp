#include "../commands.hpp"

void quit(User &user, std::vector<std::string> cmd, int fd)
{
    struct userInfo &info = user.getUserInfo(fd);
    
    // 1. Get the reason (IRC allows a custom message on quit)
    std::string reason = (cmd.size() > 1) ? cmd[1] : "Client Quit";
    if (reason[0] == ':') reason.erase(0, 1);

    // 2. Prepare the closing link message (Standard IRC)
    std::string msg = "ERROR :Closing Link: " + info.hostName + " (" + reason + ")\r\n";
    info.writeBuffer += msg;

    // 3. Mark for deletion in the next POLL cycle
    info.status = -1;

    std::cout << "FD " << fd << " is quitting: " << reason << std::endl;
}
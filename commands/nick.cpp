/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 13:46:22 by suna              #+#    #+#             */
/*   Updated: 2026/03/16 15:34:04 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../commands.hpp"

std::string prefixMsg(std::string nick, std::string login, std::string host, std::string cmd, std::string msg)
{
    std::string rst = ":";
    rst += nick;
    rst += "!";
    rst += login;
    rst += "@";
    rst += host;
    rst += " ";
    rst += cmd;
    rst += " ";
    rst += msg;
    rst += "\r\n";
    return rst;
}

bool isValidNickName(std::string Nick)
{
    size_t len = Nick.length();
    for (size_t i = 0; i < len; i++)
    {
        if (!(Nick[i] >= 65 && Nick[i] <= 126) && !(Nick[i] >= 48 && Nick[i] <= 57))
            return (false);
    }
    return (true);
}

void nick(User &user, std::vector<std::string> cmd, int fd)
{
    std::string newNick = cmd.size() > 1 ? cmd[1] : "";
    std::string nickName = user.getNickName(fd);
    std::string hostName = user.getHostName(fd);
    std::string loginName = user.getLoginName(fd);
    std::string msg = prefixMsg(nickName, loginName, hostName, "NICK", newNick);
    bool wasLogged = user.isLogin(fd);

    struct userInfo &info = user.getUserInfo(fd);
    std::string currentNick = nickName.empty() ? "*" : nickName;

    if (newNick.empty())
    {
        info.writeBuffer += ":server 431 " + currentNick + " :No nickname given\r\n";
        return;
    }
    if (!isValidNickName(newNick))
    {
        info.writeBuffer += ":server 432 " + currentNick + " " + newNick + " :Erroneous nickname\r\n";
        return;
    }
    if (user.isExist(newNick))
    {
        info.writeBuffer += ":server 433 " + currentNick + " " + newNick + " :Nickname is already in use\r\n";
        return;
    }
    user.setNickName(fd, newNick);

    if (wasLogged)
        user.setBroadCastMsg(msg);

}

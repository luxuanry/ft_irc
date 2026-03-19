/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 16:26:53 by suna              #+#    #+#             */
/*   Updated: 2026/03/18 14:56:35 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Channel.hpp"
#include <utility>
#include <iostream>
#include "commands.hpp"

std::vector<std::string> split(std::string str, std::string std)
{
    std::vector<std::string> rst;
    int std_len = std.size();
    size_t cut;
    while ((cut = str.find(std)) != std::string::npos)
    {
        if (str[0] == ':')
            break;
        std::string word = str.substr(0, cut);
        rst.push_back(word);
        str = str.substr(cut + std_len);
    }
    if (str != "")
        rst.push_back(str);
    return (rst);
}

User::User()
{

}

User::~User()
{

}

void User::initUserInfo(struct userInfo &info)
{
    info.nickName = "";
    info.loginName = "";
    info.hostName = "";
    info.writeBuffer = "";
    info.readBuffer = "";
    info.password = "";
    info.status = 0;
}

void User::addUserMapInt(int fd)
{
    userInfo info;
    initUserInfo(info);
    m_User_int.insert(std::make_pair(fd, info));
}

void User::addUserString(int fd, std::string nick)
{
    m_User_string.insert(std::make_pair(nick, fd));
}

void User::setHostName(int fd ,std::string HostName)
{
    m_User_int[fd].hostName = HostName;
}

void User::removeUser(int fd)
{
    std::string nick = m_User_int[fd].nickName;
    if (!nick.empty()) {
        m_User_string.erase(nick); 
    }
	m_User_int.erase(fd);
}

void User::setNickName(int fd, std::string nick)
{
    m_User_string.erase(m_User_int[fd].nickName);
    m_User_int[fd].nickName = nick;
    addUserString(fd, nick);
}

void User::setWrtieBuffer(int fd, std::string msg)
{
    m_User_int[fd].writeBuffer += msg;
}

void User::setBroadCastMsg(std::string msg)
{
    std::map<int, struct userInfo>::iterator it = m_User_int.begin();

    for ( ; it != m_User_int.end(); it++)
        setWrtieBuffer(it->first, msg);
}

void User::handleClientData(int fd, std::string rawInput, std::string serverPass, Channel &channel)
{
    // 1. storing the input data to readbuffer
    m_User_int[fd].readBuffer += rawInput;

    // 2. if end of buffer is \n?
    size_t pos;
    while ((pos = m_User_int[fd].readBuffer.find('\n')) != std::string::npos)
    {
        // 3. split the command which ends in \n (or \r\n)
        std::string command = m_User_int[fd].readBuffer.substr(0, pos);

        // Clean up \r if it exists
        if (!command.empty() && command[command.size() - 1] == '\r')
            command.erase(command.size() - 1);

        // 4. distinct the command with the first word
        if (!command.empty())
            executeCommand(fd, command, serverPass, channel);

        // Remove processed command from buffer
        m_User_int[fd].readBuffer.erase(0, pos + 1);
    }
}

bool User::isLogin(int fd)
{
    if (m_User_int[fd].hostName == "" || m_User_int[fd].loginName == "" || 
        m_User_int[fd].nickName == "" || m_User_int[fd].password == "")
        return (false);
    return true;
}

struct userInfo& User::getUserInfo(int fd)
{
	return m_User_int[fd];
}

bool User::isExist(std::string nickName)
{
    return(m_User_string.count(nickName));
}



void User::executeCommand(int fd, std::string cmd, std::string serverPass, Channel &channel)
{
    std::vector<std::string> cmds = split(cmd, " ");
    if (isLogin(fd))
    {
        if (cmds[0] == "JOIN")
            join(*this, channel, cmds, fd);
        else if (cmds[0] == "INVITE")
            invite(*this, channel, cmds, fd);
        else if (cmds[0] == "PRIVMSG")
            privmsg(*this, channel, cmds, fd);
        else if (cmds[0] == "NICK")
            nick(*this, cmds, fd);
		else if (cmds[0] == "PART")
			part(*this, channel, cmds, fd);
		else if (cmds[0] == "QUIT")
			quit(*this, channel, cmds, fd);
        else if (cmds[0] == "TOPIC")
            topicCmd(channel, *this, cmds, fd);
        else if (cmds[0] == "KICK")
            kickCmd(channel, *this, cmds, fd);
        else if (cmds[0] == "MODE")
            modeCmd(channel, *this, cmds, fd);
    }
    else
    {
        if (cmds[0] == "PASS")
			pass(*this, cmds, fd, serverPass);
		else if (cmds[0] == "NICK")
            nick(*this, cmds, fd);
        else if (cmds[0] == "USER")
            userCmd(*this, cmds, fd);
        
        if (isLogin(fd)){
            std::string welcome = ":server 001 " + m_User_int[fd].nickName + " :Welcome to the IRC server!\r\n";
            m_User_int[fd].writeBuffer += welcome;
            m_User_int[fd].status = 1; //change the status as connected

            std::cout << "User " << m_User_int[fd].nickName << " logged in successfully." << std::endl;
        }
    }
}

std::string User::getNickName(int fd)
{
    return m_User_int[fd].nickName;
}

std::string User::getHostName(int fd)
{
    return m_User_int[fd].hostName;
}

std::string User::getLoginName(int fd)
{
    return m_User_int[fd].loginName;
}
int User::getFdByNick(std::string nickName)
{
    if (isExist(nickName))
        return m_User_string[nickName];
    return -1;
}


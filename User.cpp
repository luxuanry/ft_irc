/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 16:26:53 by suna              #+#    #+#             */
/*   Updated: 2026/03/12 16:13:54 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include <utility>

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

void User::setHostName(int fd ,std::string HostName)
{
    m_User_int[fd].hostName = HostName;
}

void User::removeUser(int fd)
{
	m_User_int.erase(fd);
}

void User::handleClientData(int fd, std::string rawInput)
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
            executeCommand(fd, command);

        // Remove processed command from buffer
        m_User_int[fd].readBuffer.erase(0, pos + 1);
    }
}
void User::executeCommand(int fd, std::string cmd)
{
	// execute commands
}

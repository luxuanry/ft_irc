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
struct userInfo& User::getUserInfo(int fd)
{
    return m_User_int[fd];
}

User::User()
{

}

User::~User()
{

}


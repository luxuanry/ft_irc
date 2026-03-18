/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 13:25:44 by suna              #+#    #+#             */
/*   Updated: 2026/03/18 14:10:39 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::initChannelInfo(struct channelInfo &info)
{
    info.topic = "";
    info.key = "";
    info.inviteOnly = false;
    info.topicRestriced = false;
    info.hasKey = false;
    info.hasLimit = false;
    info.limit = 0;
}

void Channel::addChannel(std::string channelName)
{
    channelInfo info;
    initChannelInfo(info);
    m_channelList[channelName] = info;
}


bool Channel::isExist(std::string channelName)
{
	return (m_channelList.count(channelName) > 0);
}
  
channelInfo& Channel::getChannelInfo(std::string channelName)
{
    return m_channelList[channelName];
}

bool Channel::isUserInChannel(std::string channelName, int fd)
{
    if (!isExist(channelName))
        return false;
    return m_channelList[channelName].users.count(fd) > 0;
}

bool Channel::isOperator(std::string channelName, int fd)
{
    if (!isExist(channelName))
        return false;
    return m_channelList[channelName].operators.count(fd) > 0;
}

bool Channel::isInvited(std::string channelName, int fd)
{
    if (!isExist(channelName))
        return false;
    return m_channelList[channelName].inviteList.count(fd) > 0;
}

void Channel::addUserToChannel(std::string channelName, int fd)
{
    m_channelList[channelName].users.insert(fd);
}

void Channel::removeUserFromChannel(std::string channelName, int fd)
{
    if (isExist(channelName))
    {
        m_channelList[channelName].users.erase(fd);
        m_channelList[channelName].operators.erase(fd);
    }
}

void Channel::addOperator(std::string channelName, int fd)
{
    m_channelList[channelName].operators.insert(fd);
}

void Channel::addToInviteList(std::string channelName, int fd)
{
    m_channelList[channelName].inviteList.insert(fd);
}

std::set<int>& Channel::getUsers(std::string channelName)
{
    return m_channelList[channelName].users;
}

Channel::Channel()
{
}

Channel::~Channel()
{
}

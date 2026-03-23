/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 13:25:44 by suna              #+#    #+#             */
/*   Updated: 2026/03/23 15:52:02 by lcao             ###   ########.fr       */
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

//lcao added
void Channel::removeChannel(std::string channelName)
{
    
    m_channelList.erase(channelName); 
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

        // If channel is now empty, delete it entirely
        if (m_channelList[channelName].users.empty())
        {
            m_channelList.erase(channelName);
            return;
        }

        // If no operators remain, promote the first user
        if (m_channelList[channelName].operators.empty())
        {
            int firstUser = *(m_channelList[channelName].users.begin());
            m_channelList[channelName].operators.insert(firstUser);
        }
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

void Channel::removeFromAllInviteLists(int fd)
{
    std::map<std::string, channelInfo>::iterator it;
    for (it = m_channelList.begin(); it != m_channelList.end(); ++it)
        it->second.inviteList.erase(fd);
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

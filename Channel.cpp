/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 13:25:44 by suna              #+#    #+#             */
/*   Updated: 2026/03/12 14:41:05 by suna             ###   ########.fr       */
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

channelInfo& Channel::getChannelInfo(std::string channelName)
{
	return m_channelList[channelName];
}

bool Channel::isExist(std::string channelName)
{
	return (m_channelList.count(channelName) > 0);
}

Channel::Channel()
{
}

Channel::~Channel()
{
}
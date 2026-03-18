/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 13:25:47 by suna              #+#    #+#             */
/*   Updated: 2026/03/17 17:26:45 by lcao             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
 #define CHANNEL_HPP

#include <string>
#include <vector>
#include <set>
#include <map>

struct channelInfo
{
    std::string topic;
    bool inviteOnly;
    bool topicRestriced;
    bool hasKey;
    std::string key;
    bool hasLimit;
    int limit;
    std::set<int> inviteList;
    std::set<int> operators;
    std::set<int> users;
};

class Channel
{
    private:
        std::map<std::string, channelInfo> m_channelList;
        void initChannelInfo(channelInfo &info);
    public:
        void    addChannel(std::string channelName);
        bool 			isExist(std::string channelName);
        channelInfo& getChannelInfo(std::string channelName);
        bool    isUserInChannel(std::string channelName, int fd);
        bool    isOperator(std::string channelName, int fd);
        bool    isInvited(std::string channelName, int fd);
        void    addUserToChannel(std::string channelName, int fd);
        void    removeUserFromChannel(std::string channelName, int fd);
        void    addOperator(std::string channelName, int fd);
        void    addToInviteList(std::string channelName, int fd);
        std::set<int>& getUsers(std::string channelName);
        Channel();
        ~Channel();
    
    bool    isChannelExist(const std::string &channelName);
    bool    isUserInChannel(const std::string &channelName, int fd);
    bool    isOperator(const std::string &channelName, int fd);
        
    struct channelInfo& getChannelInfo(std::string channelName);

};

#endif

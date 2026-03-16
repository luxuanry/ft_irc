/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 16:13:46 by suna              #+#    #+#             */
/*   Updated: 2026/03/16 15:38:56 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
 #define USER_HPP

#include <string>
#include <vector>
#include <map>
#include <set>

struct userInfo
{
    std::string nickName;
    std::string loginName;
    std::string hostName;
    std::string writeBuffer;
    std::string readBuffer;
    std::string password;
    int         status;
    std::set<std::string> channelList;
};

class User
{
    private:
        std::map<int, struct userInfo> m_User_int;
        std::map<std::string, int> m_User_string;
        void    initUserInfo(struct userInfo &info);
		void	executeCommand(int fd, std::string cmd);
		
    public:
		void	handleClientData(int fd, std::string rawInput);
        void    addUserMapInt(int fd);
        void    addUserString(int fd, std::string nick);
        void    setHostName(int fd, std::string HostName);
        void    setNickName(int fd, std::string nick);
        void    removeUser(int fd);
        void    setBroadCastMsg(std::string msg);
        void    setWrtieBuffer(int fd, std::string msg);
        struct userInfo& getUserInfo(int fd);
        bool    isLogin(int fd);
        bool    isExist(std::string nickName);
        std::string getNickName(int fd);
        std::string getHostName(int fd);
        std::string getLoginName(int fd);

        User();
        ~User();
};


#endif
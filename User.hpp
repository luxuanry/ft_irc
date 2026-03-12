/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 16:13:46 by suna              #+#    #+#             */
/*   Updated: 2026/03/12 16:13:25 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    public:
        void    addUserMapInt(int fd);
        void    setHostName(int fd, std::string HostName);
        void    removeUser(int fd);
        struct userInfo& getUserInfo(int fd);
        User();
        ~User();
};


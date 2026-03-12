/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 16:13:46 by suna              #+#    #+#             */
/*   Updated: 2026/03/12 16:03:53 by lcao             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <map>

struct userInfo
{
    std::string nickName;
    std::string loginName;
    std::string hostName;
    std::string writeBuffer;
    std::string readBuffer;
    std::string password;
    int         status;
    std::vector<std::string> channelList;
};

class User
{
    private:
        std::map<int, struct userInfo> m_User_int;
        std::map<std::string, int> m_User_string;
        void    initUserInfo(struct userInfo &info);
		void	executeCommand(int fd, std::string cmd);
		
    public:
        void    addUserListInt(int fd);
		void	removeUser(int fd);
		void	handleClientData(int fd, std::string rawInput);
        struct  userInfo& getUserInfo(int fd);
        User();
        ~User();
};


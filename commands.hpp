/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 13:37:07 by suna              #+#    #+#             */
/*   Updated: 2026/03/18 13:58:34 by lcao             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
    #define COMMANDS_HPP

    #include "User.hpp"
    #include "Channel.hpp"

    void nick(User &user, std::vector<std::string> cmd, int fd);
    void pass(User &user, std::vector<std::string> cmd, int fd, const std::string serverPassword);
    void userCmd(User &userObj, std::vector<std::string> cmds, int fd);
    void modeCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd);
    void kickCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd);
    void topicCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd);
    void quit(User &user, Channel &channels, std::vector<std::string> cmd, int fd);
	void part(User &user, Channel &channels, std::vector<std::string> cmd, int fd);
    void join(User &user, Channel &channel, std::vector<std::string> cmd, int fd);
    void invite(User &user, Channel &channel, std::vector<std::string> cmd, int fd);
    void privmsg(User &user, Channel &channel, std::vector<std::string> cmd, int fd);

#endif
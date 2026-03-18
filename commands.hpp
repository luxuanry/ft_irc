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
    
    void nick(User &user, std::vector<std::string> cmd, int fd);
    void userCmd(User &userObj, std::vector<std::string> cmds, int fd);
    void modeCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd);
    void kickCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd);
    void topicCmd(Channel &chanObj, User &userObj, const std::vector<std::string> &cmds, int fd);
    

#endif
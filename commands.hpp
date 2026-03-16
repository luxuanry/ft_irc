/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 13:37:07 by suna              #+#    #+#             */
/*   Updated: 2026/03/16 16:03:33 by lcao             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
    #define COMMANDS_HPP

    #include "User.hpp"
    
    void nick(User &user, std::vector<std::string> cmd, int fd);
    void pass(User &user, std::vector<std::string> cmd, int fd, const std::string serverPassword);
    void userCmd(User &userObj, std::vector<std::string> cmds, int fd);

#endif
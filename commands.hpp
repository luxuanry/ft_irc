/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suna <suna@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 13:37:07 by suna              #+#    #+#             */
/*   Updated: 2026/03/16 13:44:24 by suna             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
    #define COMMANDS_HPP

    #include "User.hpp"
    
    void nick(User &user, std::vector<std::string> cmd, int fd);

#endif
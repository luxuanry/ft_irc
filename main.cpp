/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcao <lcao@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 12:55:23 by suna              #+#    #+#             */
/*   Updated: 2026/03/09 16:55:25 by lcao             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

void startServerLoop(Server &irc);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	try
	{
		int port = std::atoi(argv[1]);
		std::string password = argv[2];

		if (port < 1024 || port > 49151)
		{
			throw std::runtime_error("Port number must be between 1024 and 49151");
		}

		Server ircServer(port, password);
		std::cout << "Server initialized successfully." << std::endl;

		startServerLoop(ircServer);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}
	
	return 0;
}
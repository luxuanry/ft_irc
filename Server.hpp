#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <poll.h>
# include "User.hpp"

class Server
{
private:
	int 						m_socket;
	std::string 				_pass;
	std::vector<struct pollfd> 	v_fds;
	User						_userManager;


public:
	Server(int port,std::string pass);
	~Server();

	void	run();

private:
	void	_initSocket(int port);
	void	_acceptClient();
};

#endif
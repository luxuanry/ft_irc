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


public:
	Server(int port,std::string pass);
	~Server();

	// Getters
	int&						getSocket();
	std::string					getPass() const;
	std::vector<struct pollfd>&	getFds();

	std::pair<int, std::string>	acceptClient();

private:
	void	_initSocket(int port);
	
};

#endif
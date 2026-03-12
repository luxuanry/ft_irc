#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <poll.h>

class Server
{
private:
	int 						m_socket;
	std::string 				_pass;
	std::vector<struct pollfd> 	v_fds;

public:
	Server(int port,std::string pass);
	~Server();

private:
	void	_initSocket(int port);
	void	_acceptClient();
};

#endif
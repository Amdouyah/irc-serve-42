#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <deque>
#include <fcntl.h>
#include <stdexcept>
#include "Client.hpp"
#include <deque>

struct _server_
{
	int port;
	int server_socket;
	std::string password;
	struct pollfd *poll_fds; // Array of socket file descriptors
    int poll_size; // Size of descriptor array
    int poll_count; // Current number of descriptors in array
};

class Server
{

	public:

		Server(int port, std::string password);
		void start();
		int create_server_socket(void);
		void accept_new_connection(int server_socket, struct pollfd **poll_fds, int *poll_count, int *poll_size);
		void read_data_from_socket(int i, struct pollfd **poll_fds, int *poll_count, int server_socket);
		void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size);
		void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count);

		~Server();
	private:
	int 			_error;
	std::deque<Client *>	_clients;
	_server_		_server;


};

#endif /* ********************************************************** SERVER_H */
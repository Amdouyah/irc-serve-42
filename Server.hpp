#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
#include "channel.hpp"
# include <string>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_ntop
#include <unistd.h>
#include <deque>
#include <fcntl.h>
#include <stdexcept>
#include <vector>
#include "Client.hpp"
#include <deque>
#define deque_itr std::deque<Client *>::iterator
#define deque_chan std::deque<channel *>::iterator

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
		void regitration(std::vector<std::string>& lines, deque_itr& it , std::vector<std::string>::iterator& it2);
		int privmsg(std::vector<std::string>::iterator& it2, deque_itr& it);
		~Server();
	private:
	int 			_error;
	std::deque<Client *>	_clients;
	_server_		_server;
	std::deque<channel *> _channels;



};

#endif /* ********************************************************** SERVER_H */
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include "channel.hpp"
#include <string>
#include "Bot.hpp"
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
	std::deque<struct pollfd> poll_fds;
	int poll_size;	// Size of descriptor array
	int poll_count; // Current number of descriptors in array
};

class Server
{

public:
	Server(int port, std::string password);
	void start();
	int create_server_socket(void);
	void accept_new_connection(int server_socket);
	void read_data_from_socket(int i);
	void add_to_poll_fds(int new_fd);
	void del_from_poll_fds(int i);
	void regitration(std::vector<std::string> &lines, deque_itr &it, std::vector<std::string>::iterator &it2);
	static void signal_handler(int sig);
	int join(deque_itr &it, std::vector<std::string>::iterator &it2);
	int privmsg(std::vector<std::string>::iterator &it2, deque_itr &it);
	int kick_server(deque_itr &it, std::vector<std::string>::iterator &it2);
	int invite_to_channel(deque_itr &it, std::vector<std::string>::iterator &it2);
	void MODE(deque_itr &it, std::string line);
	channel *get_chan(std::string name);
	int part(deque_itr &it, std::vector<std::string>::iterator &it2);
	int nickname(deque_itr &it, std::string line);
	int mode_m(deque_itr &it, std::vector<std::string>::iterator &it2);
	int WHO(deque_itr &it, std::vector<std::string>::iterator &it2);

		~Server();

private:
	int _error;
	static bool _shutdown;
	Bot _bot;
	std::deque<Client *> _clients;
	_server_ _server;
	std::deque<channel *> _channels;
};

#endif /* ********************************************************** SERVER_H */
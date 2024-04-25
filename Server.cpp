#include "Server.hpp"

void Server::signal_handler(int sig)
{
	(void)sig;
	Server::_shutdown = true;
}

int Server::create_server_socket(void)
{
	struct sockaddr_in sa;
	int socket_fd;
	int status;

	// Prepare the address and port for the server socket
	std::memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;					 // IPv4
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
	sa.sin_port = htons(this->_server.port);
	// Create listening socket
	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if (socket_fd == -1)
		throw(std::runtime_error("faild to create socket"));
	int en = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	std::cout << "[Server] Created server socket fd: " << socket_fd << std::endl;
	// Bind socket to address and port
	status = bind(socket_fd, (struct sockaddr *)&sa, sizeof sa);
	if (status != 0)
	{
		std::cerr << "[Server] Bind error: " << strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "[Server] Bound server socket to port " << this->_server.port << std::endl;
	std::cout << "[Server] Listening on port " << this->_server.port << std::endl;
	status = listen(socket_fd, 10);
	if (status != 0)
	{
		std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
		return -1;
	}
	return (socket_fd);
}

Server::Server(int port, std::string password)
{
	this->_shutdown = false;
	this->_error = 0;
	this->_server.port = port;
	this->_server.password = password;
}

void Server::start()
{
	int status;
	struct pollfd poll;
	this->_server.server_socket = create_server_socket();
	if (this->_server.server_socket == -1)
		throw std::runtime_error("[Server] Error creating server socket");
	poll.fd = this->_server.server_socket;
	poll.events = POLLIN;
	this->_server.poll_count = 1;
	this->_server.poll_fds.push_back(poll);
	while (1)
	{
		if (Server::_shutdown)
		{
			std::cout << "[Server] Shutting down..." << std::endl;
			close(this->_server.server_socket);
			break;
		}
		status = ::poll(&this->_server.poll_fds[0], this->_server.poll_count, 2000);
		if (status == -1)
		{
			if (errno == EINTR)
				continue;
			else
				throw std::runtime_error("[Server] Poll error: " + std::string(strerror(errno)));
		}
		else if (status == 0)
			continue;

		for (int i = 0; i < this->_server.poll_count; i++)
		{
			if ((this->_server.poll_fds[i].revents & POLLIN))
			{
				if (this->_server.poll_fds[i].fd == this->_server.server_socket)
				{
					accept_new_connection(this->_server.server_socket);
				}
				else
				{
					read_data_from_socket(i);
				}
			}
		}
	}
}

Server::~Server()
{
}

void Server::accept_new_connection(int server_socket)
{
	int client_fd;
	int status;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len); // Accept new connection

	if (client_fd == -1)
		throw std::runtime_error("[Server] Accept error: " + std::string(strerror(errno)));
	fcntl(client_fd, F_SETFL, O_NONBLOCK); // Set non-blocking
	add_to_poll_fds(client_fd);
	// Send welcome message to client
	std::string msg_to_send = "Welcome to irc server \n";
	status = send(client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
	if (status == -1)
		throw std::runtime_error("[Server] Send error to client fd " + std::to_string(client_fd) + ": " + std::string(strerror(errno)));
	Client *new_client = new Client();
	new_client->client_fd = client_fd;
	new_client->client_addr = client_addr;												 // Save client address
	inet_ntop(AF_INET, &(client_addr.sin_addr), new_client->client_ip, INET_ADDRSTRLEN); // Fix: Include the necessary header file
	new_client->state = 0;
	_clients.push_back(new_client);
}


void Server::regitration(std::vector<std::string> &lines, deque_itr &it, std::vector<std::string>::iterator &it2)
{
	for (; it2 != lines.end(); it2++)
	{
		if ((*it2).find("PASS") == 0)
		{
			std::string testing = std::string(*it2).substr(5, this->_server.password.length());
			if (testing == this->_server.password)
				(*it)->password = true;
			else
				std::cout << "password is incorrect" << std::endl;
		}
		else if ((*it)->password == true)
		{
			if ((*it)->registered == false)
			{
				if ((*it2).find("NICK") == 0)
					(*it)->nickname = std::string(*it2).substr(5);
				else if ((*it2).find("USER") == 0)
				{
					int i = 0;
					std::stringstream sss((*it2).substr(5));
					std::string parm;
					std::vector<std::string> parms;
					std::vector<std::string> final;
					while (std::getline(sss, parm, ' '))
						parms.push_back(parm);
					std::vector<std::string>::iterator it3 = parms.begin();
					while ((*it3).find(":") != 0 && it3 != parms.end())
					{
						i++;
						final.push_back(*it3);
						it3++;
					}
					if (i != 3 || (*it3).find(":") == std::string::npos)
					{
						std::cout << "you need more parametre" << std::endl;
					}
					else
					{
						std::string realname;
						while (it3 != parms.end())
						{
							realname += *it3;
							it3++;
						}
						final.push_back(realname);
						(*it)->username = final[0];
						(*it)->hostname = final[1];
						(*it)->servername = final[2];
						(*it)->realname = final[3];
					}
				}
				else
					std::cout << "you need to register" << std::endl;
				if ((*it)->nickname.size() > 0 && (*it)->username.size() > 0)
				{
					(*it)->registered = true;
					std::cout << "you are registered" << std::endl;
				}
			}
		}
		else
			std::cout << "you need password to connect to server" << std::endl;
	}
}

int Server::privmsg(std::vector<std::string>::iterator &it2, deque_itr &it)
{
	int dest_fd;
	int status;
	if ((*it2).find("PRIVMSG") == 0)
	{
		std::string dest = std::string(*it2).substr(8, std::string(*it2).find(" ", 8) - 8);
		std::string msg = std::string(*it2).substr(std::string(*it2).find(":", 8) + 1);
		if (dest.find("#") == 0)
		{
			for (deque_chan chan = _channels.begin(); chan != _channels.end(); chan++)
			{
				if ((*chan)->get_name() == dest.substr(1))
				{
					for (deque_itr it3 = (*chan)->alpha_users.begin(); it3 != (*chan)->alpha_users.end(); it3++)
					{
						std::string msg_to_send = (*it)->nickname + " : " + msg + "\n";
						status = send((*it3)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
						if (status == -1)
							throw std::runtime_error("[Server] Send error to client fd " + std::to_string((*it3)->client_fd) + ": " + std::string(strerror(errno)));
					}
					for (deque_itr it3 = (*chan)->beta_users.begin(); it3 != (*chan)->beta_users.end(); it3++)
					{
						std::string msg_to_send = (*it)->nickname + " : " + msg + "\n";
						status = send((*it3)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
						if (status == -1)
							throw std::runtime_error("[Server] Send error to client fd " + std::to_string((*it3)->client_fd) + ": " + std::string(strerror(errno)));
					}
					break;
				}
			}
		}
		else if (dest == "jhonny")
		{

			std::string msg_to_send = this->_bot.game(it, msg);
			status = send((*it)->client_fd, (msg_to_send.c_str()), msg_to_send.length(), 0);
			if (status == -1)
				throw std::runtime_error("[Server] Send error to client fd " + std::to_string(1) + ": " + std::string(strerror(errno)));
		}
		else
		{
			for (deque_itr it3 = _clients.begin(); it3 != _clients.end();)
			{
				if ((*it3)->nickname == dest)
				{
					dest_fd = (*it3)->client_fd;
					break;
				}
				it3++;
				if (it3 == _clients.end())
					dest_fd = 0;
			}
			if (dest_fd == 0)
				std::cout << "user not found" << std::endl;
			else
			{
				std::string msg_to_send = (*it)->nickname + " : " + msg + "\n";
				status = send(dest_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
				if (status == -1)
					throw std::runtime_error("[Server] Send error to client fd " + std::to_string(dest_fd) + ": " + std::string(strerror(errno)));
			}
		}
		return 1;
	}
	return 0;
}

int Server::kick_server(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	int i = 0;
	int a = 0;
	if ((*it2).find("KICK") == 0)
	{
		while (1)
		{
			a = (*it2).find(" ", a + 1);
			i++;
			if (a == std::string::npos || i >= 2)
				break;
		}
		if (i != 2)
		{
			std::cout << "you need more parametre" << std::endl;
			return 1;
		}
		std::string channnel_name = std::string((*it2).substr((*it2).find("#", 5) + 1, (*it2).find(" ", 5) - ((*it2).find("#", 5) + 1)));
		std::string target = std::string((*it2).substr((*it2).find(" ", 5) + 1));
		deque_chan chanel = this->_channels.begin();
		bool found = false;
		for (; chanel != this->_channels.end(); chanel++)
		{
			if ((*chanel)->get_name() == channnel_name.c_str())
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			std::cout << "channel not found" << std::endl;
			return 1;
		}
		// std::string msg_to_send = (*chanel)->KICK(*it, *kicker);
		return 1;
	}

	return 0;
}

int Server::invite_to_channel(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("INVITE") == 0)
	{
		std::string nicknam = std::string((*it2).substr(7, (*it2).find(" ", 7) - 7));
		std::string channnel_name = std::string((*it2).substr((*it2).find("#") + 1, (*it2).length() - (*it2).find("#") - 1));
		bool found = false;
		deque_chan it3 = _channels.begin();
		for(; it3 != _channels.end(); it3++)
		{
			if ((*it3)->get_name() == channnel_name)
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			std::cout << "channel not found" << std::endl;
			return 1;
		}
		for (deque_itr it4 = _clients.begin(); it4 != _clients.end(); it3++)
		{
			if ((*it4)->nickname == nicknam)
			{
				std::cout << "do the invite" << std::endl;
				// (*it3)->INVITE(*it, *it4);
				break;
			}
		}
		return 1;
	}
	return 0;
}

int Server::join(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("JOIN") == 0)
	{
		if ((*it2).find("#") == 5)
		{
			bool found = false;
			if (_channels.size() > 0)
			{
				for (deque_chan chan = _channels.begin(); chan != _channels.end(); chan++)
				{
					if ((*chan)->get_name() == std::string(*it2).substr(6))
					{
						found = true;
						if ((*chan)->invitOnly == true)
						{
							for (deque_itr it3 = (*chan)->invited.begin(); it3 != (*chan)->invited.end(); it3++)
							{
								if ((*it3)->nickname == (*it)->nickname)
								{
									(*chan)->beta_users.push_back(*it); // adding alpha user to containers in server
									break;
								}
							}
							std::cout << "you are not invited" << std::endl;
						}
						else if ((*chan)->invitOnly == false)
						{
							(*chan)->beta_users.push_back(*it); // adding alpha user to containers in server
						}
						break;
					}
				}
			}
			if (found == false)
			{
				std::string channel_name = std::string(*it2).substr(6);
				channel *new_channel = new channel(channel_name);
				new_channel->alpha_users.push_back(*it);
				new_channel->beta_users.push_back(*it);
				std::cout << "channel created"
						  << "with name " << new_channel->_name << std::endl;
				_channels.push_back(new_channel);
			}
		}
		else
			std::cout << "channel name must start with #" << std::endl;
		return 1;
	}
	return 0;
}

void Server::read_data_from_socket(int i)
{
	char buffer[BUFSIZ];
	char msg_to_send[BUFSIZ];
	int bytes_read;
	int sender_fd;
	sender_fd = this->_server.poll_fds[i].fd;
	std::memset(&buffer, '\0', sizeof buffer); // Clear the buffer
	bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);
	buffer[bytes_read] = '\0';
	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
		{
			std::cout << "[Server] Client fd " << sender_fd << " disconnected." << std::endl;
			close(sender_fd); // Close socket
			for (deque_itr it = _clients.begin(); it != _clients.end(); it++)
			{
				if ((*it)->client_fd == sender_fd)
				{
					delete *it;
					_clients.erase(it);
					break;
				}
			}
			del_from_poll_fds(i);
		}
	}
	else
	{
		deque_itr it = _clients.begin();
		for (it = _clients.begin(); it != _clients.end(); it++)
		{
			if ((*it)->client_fd == sender_fd)
				break;
		}
		std::stringstream ss(buffer);
		std::string line;
		std::vector<std::string> lines;
		while (std::getline(ss, line, '\n'))
			lines.push_back(line);
		std::vector<std::string>::iterator it2 = lines.begin();
		if ((*it)->registered == false)
			regitration(lines, it, it2);
		else
		{
			for (; it2 != lines.end(); it2++)
			{
				if (privmsg(it2, it) == 1)
					continue;
				else if (join(it, it2) == 1)
					continue;
				else if (kick_server(it, it2) == 1)
					continue;
				else if (invite_to_channel(it, it2) == 1)
					continue;
				else
					std::cout << "command unkown" << std::endl;
			}
		}
	}
}
void Server::add_to_poll_fds(int new_fd)
{
	struct pollfd new_poll_fds;
	new_poll_fds.fd = new_fd;
	new_poll_fds.events = POLLIN;
	this->_server.poll_count++;
	this->_server.poll_fds.push_back(new_poll_fds);
}
void Server::del_from_poll_fds(int i)
{
	this->_server.poll_fds.erase(this->_server.poll_fds.begin() + i);
	this->_server.poll_count--;
}

channel* Server::get_chan(std::string name){
	for (size_t i = 0; i < _channels.size(); ++i) {
		if (_channels[i]->get_name() == name)
			return _channels[i];
	}
	return NULL;
}


void Server::MODE(deque_itr &it, std::string line){
	std::istringstream input(line);

	std::string channel_n, mode, param;
	input >> channel_n >> mode >> param; 
	channel* chan = get_chan(channel_n);
	if(chan){
		chan->MODE(*it, mode, param);
	}
	else {
		//print not such channel
	}
	//check if channel valid 

}
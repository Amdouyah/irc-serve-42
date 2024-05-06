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
	client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
	fcntl(client_fd, F_SETFL, O_NONBLOCK); // Set non-blocking
	add_to_poll_fds(client_fd);
	Client *new_client = new Client();
	new_client->client_fd = client_fd;
	new_client->client_addr = client_addr;
	new_client->ip_adress = inet_ntop(AF_INET, &(client_addr.sin_addr), new_client->client_ip, INET_ADDRSTRLEN);
	new_client->state = 0;
	_clients.push_back(new_client);
}

void Server::regitration(std::vector<std::string> &lines, deque_itr &it, std::vector<std::string>::iterator &it2)
{
	for (; it2 != lines.end(); it2++)
	{
		if ((*it2).find("CAP") == 0)
			continue;
		if ((*it2).find("PASS") == 0)
		{
			std::stringstream ss(*it2);

			std::string pass;
			ss >> pass >> pass;
			if (pass == this->_server.password)
			{
				(*it)->password = true;
			}
			else
			{
				std::string msg_to_send = ERR_PASSWDMISMATCH();
				send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
			}
		}
		else if ((*it)->password == true)
		{
			if ((*it)->registered == false)
			{
				if ((*it2).find("NICK") == 0)
				{
					std::stringstream ss(*it2);
					std::string nick;
					ss >> nick >> nick;
					deque_itr clientPre = _clients.begin();
					for (; clientPre != _clients.end(); clientPre++)
					{
						if ((*clientPre)->nickname == nick)
						{
							std::string msg_to_send = channel::getUserInfo(*it, 0) + " " + (*it)->nickname + " " + nick + ":Nickname is already in use\r\n";
							send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
							return;
						}
					}
					(*it)->nickname = nick;
				}
				else if ((*it2).find("USER") == 0)
				{
					std::stringstream ss(*it2);
					std::string parm;
					std::string parm1;
					std::string parm2;
					std::string parm3;
					std::string realname;
					ss >> parm >> parm >> parm1 >> parm2;
					while (ss >> parm3)
						realname += parm3 + " ";
					if (realname.find(":") != 0)
					{
						std::string msg_to_send = ERR_NEEDMOREPARAMS1();
						send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
					}
					else
					{
						(*it)->username = parm;
						(*it)->hostname = parm1;
						(*it)->servername = parm2;
						(*it)->realname = realname.substr(1);
					}
				}
				if ((*it)->nickname.size() > 0 && (*it)->username.size() > 0)
				{
					std::string msg_to_send = RPL_WELCOME();
					send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
					(*it)->registered = true;
				}
			}
		}
		else
		{
			std::string msg_to_send = ERR_PASSWDMISMATCH();
			send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
		}
	}
}

int Server::privmsg(std::vector<std::string>::iterator &it2, deque_itr &it)
{
	if ((*it2).find("PRIVMSG") == 0)
	{
		int dest_fd;
		std::string line = *it2;
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (ss >> word)
			words.push_back(word);
		std::string dest = words[1];
		std::string msg;
		for (int i = 2; i < words.size(); i++)
			msg += " " + words[i];
		if (dest.find("#") == 0)
		{
			for (deque_chan chan = _channels.begin(); chan != _channels.end(); chan++)
			{
				if ((*chan)->get_name() == dest)
				{
					if (!(*chan)->onChannel(*it))
					{
						msg = (*chan)->getUserInfo((*it), false) + ERR_NOTONCHANNEL((*it)->nickname, (*chan)->get_name());
						channel::setbuffer(msg, (*it)->client_fd);
						return 0;
					}
					for (deque_itr it3 = (*chan)->beta_users.begin(); it3 != (*chan)->beta_users.end(); ++it3)
					{
						if ((*it3)->nickname == (*it)->nickname)
							continue;
						std::string msg_to_send = channel::getUserInfo(*it, 1) + "PRIVMSG " + (*chan)->get_name() + msg + "\r\n";
						send((*it3)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
					}
					break;
				}
			}
		}
		else if (dest == "jhonny")
		{
			std::string msg_to_send = ":jhonny!jhonny@localhost PRIVMSG " + (*it)->nickname + " : " + this->_bot.game(it, msg) + "\r\n";
			send((*it)->client_fd, (msg_to_send.c_str()), msg_to_send.length(), 0);
		}
		else
		{
			deque_itr it3 = _clients.begin();
			for (; it3 != _clients.end(); it3++)
			{
				if ((*it3)->nickname == dest)
				{
					dest_fd = (*it3)->client_fd;
					break;
				}
			}
			if (it3 == _clients.end())
				dest_fd = 0;
			if (dest_fd == 0)
			{
				std::string msg_to_send = channel::getUserInfo(*it, 1) + ERR_NOSUCHNICK((*it)->nickname, dest);
				send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
			}
			else
			{
				std::string msg_to_send = channel::getUserInfo(*it, 1) + "PRIVMSG " + (*it3)->nickname + msg + "\r\n";
				send(dest_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
			}
		}
		return 1;
	}
	return 0;
}

int Server::kick_server(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("KICK") == 0)
	{
		std::string line = *it2;
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (ss >> word)
			words.push_back(word);

		std::string channel_name = words[1];
		std::string target = words[2];
		std::string reason;
		for (int i = 3; i < words.size(); i++)
			reason += words[i] + " ";
		deque_chan chanel = this->_channels.begin();
		bool found = false;
		for (; chanel != this->_channels.end(); chanel++)
		{
			if ((*chanel)->get_name() == channel_name)
			{
				found = true;
				for (deque_itr trgt = this->_clients.begin(); trgt != this->_clients.end(); trgt++)
				{
					if ((*trgt)->nickname == target)
					{
						(*chanel)->KICK(*it, *trgt, reason);
						break;
					}
				}
				break;
			}
		}
		if (found == false)
		{
			std::string msg_to_send = ERR_NOSUCHCHANNEL((*it)->nickname, channel_name);
			send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
			return 1;
		}
		return 1;
	}
	return 0;
}

int Server::invite_to_channel(deque_itr &it, std::vector<std::string>::iterator &it2)
{

	if ((*it2).find("INVITE") == 0)
	{
		std::string line = *it2;
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (ss >> word)
			words.push_back(word);
		std::string channel_name = words[1];
		std::string nicknam = words[2];
		bool found = false;
		deque_chan it3 = _channels.begin();
		for (; it3 != _channels.end(); it3++)
		{
			if ((*it3)->get_name() == channel_name)
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			std::string msg_to_send = ERR_NOSUCHCHANNEL((*it)->nickname, channel_name);
			send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
			return 1;
		}
		for (deque_itr it4 = _clients.begin(); it4 != _clients.end(); it3++)
		{
			if ((*it4)->nickname == nicknam)
			{

				// ************ hadchi ba9i matesstac

				(*it3)->INVITE(*it, *it4);
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
		std::string line = *it2;
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (ss >> word)
			words.push_back(word);
		std::string channel_name = words[1];
		std::string password = words[2];
		if (channel_name.find("#") == 0)
		{
			bool found = false;
			if (_channels.size() > 0)
			{
				for (deque_chan chan = _channels.begin(); chan != _channels.end(); chan++)
				{
					if ((*chan)->get_name() == channel_name)
					{
						found = true;
						bool invite = (*chan)->Invit_Only();
						bool has_password = (*chan)->HasPass();
						bool limits = (*chan)->HasLimit();

						if (invite)
						{
							deque_itr it3 = (*chan)->invited.begin();
							for (; it3 != (*chan)->invited.end(); it3++)
							{
								if ((*it3)->nickname == (*it)->nickname)
								{
									invite = false;
									break;
								}
							}
						}
						if (has_password)
						{
							if (password == (*chan)->get_pass())
								has_password = false;
						}
						if (limits)
						{
							if ((*chan)->get_maxUsers() > (*chan)->beta_users.size())
								limits = false;
						}
						if (!limits && !has_password && !invite)
						{
							(*chan)->beta_users.push_back(*it); // adding alpha user to containers in server
							std::string msg_to_send = channel::getUserInfo((*it), 1) + " JOIN " + (*chan)->get_name() + "\r\n";
							for (std::deque<Client *>::iterator betaUser = (*chan)->beta_users.begin(); betaUser != (*chan)->beta_users.end(); betaUser++)
								send((*betaUser)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
						}
						else
						{
							if (limits)
							{
								std::string msg_to_send = channel::getUserInfo((*it), 0) + "471 " + (*it)->nickname + " " + (*chan)->get_name() + " :Cannot join channel (+l)\r\n";
								send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
							}
							if (has_password)
							{
								std::string msg_to_send = channel::getUserInfo((*it), 0) + "475 " + (*it)->nickname + " " + (*chan)->get_name() + " :Cannot join channel (+k)\r\n";
								send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
							}
							if (invite)
							{
								std::string msg_to_send = channel::getUserInfo((*it), 0) +"473 " +  (*it)->nickname + " " + (*chan)->get_name() + ":Cannot join channel (+i)\r\n";
								send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
							}
						}
						break;
					}
				}
			}
			if (found == false)
			{
				time_t currentTime;
    			time(&currentTime);
				channel *new_channel = new channel(channel_name);
				std::string timeString = ctime(&currentTime);
				new_channel->creation_time = timeString;
				new_channel->alpha_users.push_back(*it);
				new_channel->beta_users.push_back(*it);
				std::string msg_to_send = channel::getUserInfo((*it), 1) + " JOIN " + new_channel->get_name() + "\r\n";
				send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
				_channels.push_back(new_channel);
			}
		}
		else
		{
			std::string msg_to_send = channel::getUserInfo((*it), 1) + ERR_NOSUCHCHANNEL((*it)->nickname, std::string(*it2).substr(6)) + "\r\n";
			send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
		}
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
				else if (part(it, it2) == 1)
					continue;
				else if (mode_m(it, it2) == 1)
					continue;
				else if (WHO(it, it2) == 1)
					continue;
				else if(topic_m(it, it2) == 1)
					continue;
				else if(nickname(it, *it2) == 1)
					continue;
				else
				{
					std::string msg_send = channel::getUserInfo(*it, 0) + ERR_UNKNOWNCOMMAND((*it)->nickname, (*it2));
					send((*it)->client_fd, msg_send.c_str(), msg_send.length(), 0);
				}
			}
		}
	}
}

int Server::WHO(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("WHO") == 0)
	{
		std::string line = *it2;
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (ss >> word)
			words.push_back(word);
		std::string channel_name = words[1];
		deque_chan chan = _channels.begin();
		for (; chan != _channels.end(); chan++)
		{
			if ((*chan)->get_name() == channel_name)
			{
				(*chan)->rpl_who(*it);
				break;
			}
		}
		return 1;
	}
	return 0;
}

int Server::mode_m(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("MODE") == 0)
	{
		std::string evrything = std::string((*it2).substr(5));
		MODE(it, evrything);
		return 1;
	}
	return 0;
}
int Server::topic_m(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("TOPIC") == 0)
	{
		std::string evrything = std::string((*it2).substr(5));
		_Topic(it, evrything);
		return 1;
	}
	return 0;
}


int Server::part(deque_itr &it, std::vector<std::string>::iterator &it2)
{
	if ((*it2).find("PART") == 0)
	{
		std::string line = *it2;
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (ss >> word)
			words.push_back(word);
		std::string channel_name = words[1];
		std::string reason = words[2];
		deque_chan chan = _channels.begin();
		for (; chan != _channels.end(); chan++)
		{
			if ((*chan)->get_name() == channel_name)
			{
				(*chan)->PART(*it, reason);
				break;
			}
		}
		if (chan == _channels.end())
		{
			std::string msg_to_send = ERR_NOSUCHCHANNEL((*it)->nickname, channel_name);
			send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
		}
		return 1;
	}
	return 0;
}
int Server::nickname(deque_itr &it, std::string line)
{
	if (line.find("NICK") == 0)
	{
		std::stringstream ss(line);
		std::string nick;
		ss >> nick >> nick;
		deque_itr it2 = _clients.begin();
		for (; it2 != _clients.end(); it2++)
		{
			if ((*it2)->nickname == nick)
			{
				std::string msg_to_send = channel::getUserInfo(*it, 0) + " " + (*it)->nickname + " " + nick + ":Nickname is already in use\r\n";
				send((*it)->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
				return 1;
			}
		}
		std::string send_msg = channel::getUserInfo(*it, 0) + "NICK :" + nick;
		for(deque_itr client= _clients.begin(); client != _clients.end(); client++)
		{
			if((*it)->nickname == (*client)->nickname)
				continue;
			send((*client)->client_fd, send_msg.c_str(), send_msg.length(), 0);
		}
		(*it)->nickname = nick;
		return 1;
	}
	return 0;
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

channel *Server::get_chan(std::string name)
{
	for (size_t i = 0; i < _channels.size(); ++i)
	{
		if (_channels[i]->get_name() == name)
			return _channels[i];
	}
	return NULL;
}

void Server::MODE(deque_itr &it, std::string line)
{
	std::istringstream input(line);

	std::string channel_n, mode, param;
	input >> channel_n >> mode >> param;
	channel *chan = get_chan(channel_n);
	if (chan)
		chan->MODE(*it, mode, param);
	else
		channel::setbuffer(channel::getUserInfo((*it), false) + ERR_NOSUCHCHANNEL((*it)->nickname, chan->get_name()), (*it)->client_fd);
}
void Server::_Topic(deque_itr &it, std::string line)
{
	std::string channel_n, param, result;
	std::istringstream input(line);
	input >> channel_n;
	while (input){
		input >> param;
		result += param + " ";
		param.clear();
	}
	channel *chan = get_chan(channel_n);
	if (chan)
		chan->TOPIC((*it), result);
	else
		channel::setbuffer(channel::getUserInfo((*it), false) + ERR_NOSUCHCHANNEL((*it)->nickname, chan->get_name()), (*it)->client_fd);
}
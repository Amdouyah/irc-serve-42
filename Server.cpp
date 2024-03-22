#include "Server.hpp"

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
	this->_error = 0;
	this->_server.port = port;
	this->_server.password = password;
}

void Server::start()
{
	int status;
	this->_server.server_socket = create_server_socket();
	if (this->_server.server_socket == -1)
		throw std::runtime_error("[Server] Error creating server socket");
	this->_server.poll_size = 5;
	this->_server.poll_fds = (struct pollfd *)calloc(this->_server.poll_size + 1, sizeof *this->_server.poll_fds);
	if (!this->_server.poll_fds)
		throw std::runtime_error("[Server] Error allocating memory for poll_fds");
	this->_server.poll_fds[0].fd = this->_server.server_socket;
	this->_server.poll_fds[0].events = POLLIN;
	this->_server.poll_count = 1;
	while (1)
	{
		status = poll(this->_server.poll_fds, this->_server.poll_count, 2000);
		if (status == -1)
			throw std::runtime_error("[Server] Poll error: " + std::string(strerror(errno)));

		else if (status == 0)
			continue;

		for (int i = 0; i < this->_server.poll_count; i++)
		{
			if ((this->_server.poll_fds[i].revents & POLLIN) != 1)
			{
				continue;
			}
			if (this->_server.poll_fds[i].fd == this->_server.server_socket)
			{
				accept_new_connection(this->_server.server_socket, &this->_server.poll_fds, &this->_server.poll_count, &this->_server.poll_size);
			}
			else
			{
				read_data_from_socket(i, &this->_server.poll_fds, &this->_server.poll_count, this->_server.server_socket);
			}
		}
	}
}

Server::~Server()
{
}

void Server::accept_new_connection(int server_socket, struct pollfd **poll_fds, int *poll_count, int *poll_size)
{
	int client_fd;
	int status;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len); // Accept new connection

	if (client_fd == -1)
		throw std::runtime_error("[Server] Accept error: " + std::string(strerror(errno)));
	fcntl(client_fd, F_SETFL, O_NONBLOCK); // Set non-blocking
	add_to_poll_fds(poll_fds, client_fd, poll_count, poll_size);
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
//****************************************************************


		
void Server::regitration(std::vector<std::string>& lines, deque_itr& it)
{
	for (std::vector<std::string>::iterator it2 = lines.begin(); it2 != lines.end(); it2++)
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
						else{
						std::string realname;
						while (it3 != parms.end())
						{
							realname += *it3;
							it3++;
						}
						final.push_back(realname);
						(*it)->username = final[0];
						(*it)->realname = final[3];
						(*it)->registered = true;
						std::cout << "you are registered" << std::endl;
						}
					}
					else
						std::cout << "you need to register" << std::endl;
				}
			}
			else
				std::cout << "you need password to connect to server" << std::endl;
		}

}

void Server::read_data_from_socket(int i, struct pollfd **poll_fds, int *poll_count, int server_socket)
{
	char buffer[BUFSIZ];
	char msg_to_send[BUFSIZ];
	int bytes_read;
	int status;
	int dest_fd;
	int sender_fd;
	sender_fd = (*poll_fds)[i].fd;
	std::memset(&buffer, '\0', sizeof buffer); // Clear the buffer
	bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);
	if (bytes_read < 0)
	{
			
		if (errno == EAGAIN || errno == EWOULDBLOCK){}// wa7ad mochkila dial fach katbghi tconnecta awtani kigolo lik resourse not available prabably 9aditha be socketopt walakine jat to be sure 
		else
		{
			close(sender_fd); // Close socket
			del_from_poll_fds(poll_fds, i, poll_count);
			throw std::runtime_error("[Server] Receive error from client fd " + std::to_string(sender_fd) + ": " + std::string(strerror(errno)));
		}
	}
	else if (bytes_read == 0)
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
		del_from_poll_fds(poll_fds, i, poll_count);
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
		regitration(lines, it);
		
	}
}
//****************************************************************
void Server::add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size)
{
	// If there is not enough room, reallocate the poll_fds array
	if (*poll_count == *poll_size)
	{
		*poll_size *= 2; // Double its size
		*poll_fds = (pollfd *)realloc(*poll_fds, sizeof(**poll_fds) * (*poll_size));
	}
	(*poll_fds)[*poll_count].fd = new_fd;
	(*poll_fds)[*poll_count].events = POLLIN;
	(*poll_count)++;
}
void Server::del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count)
{
	// Copy the fd from the end of the array to this index
	(*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
	(*poll_count)--;
}

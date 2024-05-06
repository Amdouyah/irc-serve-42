#pragma once

#include <cstring>
#include <iostream>
#include <string>
#include <netinet/in.h> // sockaddr_in


class Client {
    public:
    int client_fd;
    int state;
    std::string username;
    std:: string nickname;
    std::string hostname;
    std::string servername;
    std::string realname;
    std::string ip_adress;
    std::string command;
    bool password;
    bool registered;
	struct sockaddr_in client_addr;
    char client_ip[INET_ADDRSTRLEN];
    Client();

};
#pragma once

#include <cstring>
#include <iostream>
#include <string>
#include <netinet/in.h>

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
    bool playing_game;
    bool password;
    bool registered;
	struct sockaddr_in client_addr;
    char client_ip[INET_ADDRSTRLEN];
    Client();

};
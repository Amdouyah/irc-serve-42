#include <cstring>
#include <iostream>
#include <string>

class Client {
    public:
    int client_fd;
    int state;
    std::string username;
    std:: string nickname;
    std::string realname;
    Client();

};
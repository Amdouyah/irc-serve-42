#include "Client.hpp"

Client::Client()
{
    this->client_fd = -1;
    this->state = 0;
    this->password = false;
    this->username = "";
    this->nickname = "";
    this->realname = "";
}
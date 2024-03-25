#include "Client.hpp"

Client::Client()
{
    this->client_fd = -1;
    this->state = 0;
    this->password = false;
    this->registered = false;
    this->username = "";
    this->nickname = "";
    this->realname = "";
    this->hostname = "";
    this->servername = "";
}
//khass t3mer ela had chkel : USERname ,  hostname , servername, realname;
//fash tparsi USER ghady ikhsni hta lhost o server
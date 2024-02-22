#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[255];
    if(argc < 3)
    {
        std::cerr << "usage " << argv[0] << " hostname port" << "\n";
        return 0;
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "Error opening socket" << "\n";
        return 0;
    }
    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
        std::cerr << "Error, no such host" << "\n";
        return 0;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed" << "\n";
        return 0;
    }

    while(1)
    {
        bzero(buffer, 255);
        std::cin.getline(buffer, 255);
        n = write(sockfd, buffer, strlen(buffer));
        if(n < 0)
        {
            std::cerr << "Error on writing" << "\n";
            return 0;
        }
        bzero(buffer, 255);
        n = read(sockfd, buffer, 255);
        if(n < 0)
        {
            std::cerr << "Error on reading" << "\n";
            return 0;
        }
        std::cout << "Server: " << buffer << "\n";
        if(strcmp(buffer, "bye") == 0)
        {
            break;
        }
    }

    return 0;
}
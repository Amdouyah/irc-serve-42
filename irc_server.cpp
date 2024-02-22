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
    if(argc < 2)
    {
        std::cerr << "port not provided,  program existed" << "\n";
        return 1;
    }

    int sockfd, newsockfd, portno, n;
    char buffer[255];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "Error opening socket" << "\n";
        return 1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error on binding" << "\n";
        return 1;
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0)
    {
        std::cerr << "Error on accept" << "\n";
        return 1;
    }
    while(1)
    {
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if(n < 0)
        {
            std::cerr << "Error reading from socket" << "\n";
            return 1;
        }
        std::cout << "Client: " << buffer << "\n";
        bzero(buffer, 255);
        std::cout << "Server: ";
        std::string data;
        std::getline(std::cin, data);
        strcpy(buffer, data.c_str());
        n = write(newsockfd, buffer, strlen(buffer));
        if(n < 0)
        {
            std::cerr << "Error writing to socket" << "\n";
            return 1;
        }
        int i = strncmp("Bye", buffer, 3);
        if(i == 0)
            break;
    }
    close(newsockfd);  
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hassimi <hassimi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 16:28:04 by hassimi           #+#    #+#             */
/*   Updated: 2024/03/17 14:41:32 by hassimi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include <sstream>



int main(int ac, char **av)
#include <sstream>

{
    if(ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return (1);
    }
    // create a serevr socket
    std::stringstream ss;
    ss << av[1];
    int port;
    ss >> port;    
    Server server(port, av[2]);

    try
    {
        server.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}
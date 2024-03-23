#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include "Client.hpp"
#include <deque>

class channel{
	public:
		std::string _name;
		int	 		users;
		int 		passwd;
		bool 		has_pass;
		std::string Topic;
		bool 		has_topic;
		std::deque<Client *> alpha_users;
		std::deque<Client *> beta_users;

		channel();
		channel(std::string name);
		~channel();
};


#endif
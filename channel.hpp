#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>

class channel{
	private:
		std::string _name;
		int	 		users;
		int 		passwd;
		bool 		has_pass;
		std::string Topic;
		bool 		has_topic;
		

		channel();

	public:
		channel(std::string name);
		~channel();
};


#endif
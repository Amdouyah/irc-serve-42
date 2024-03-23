#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <deque>
#include "Client.hpp"

class channel{
	public:
		std::string _name;
		bool		invitOnly;

		int	 		users;

		bool 		has_pass;
		std::string 		passwd;

		std::string Topic;
		bool 		has_topic;
		
		Client 		creat;
		
		bool		limitsuser;
		int			maxUsers;



		channel();


		std::deque<Client *> alpha_users;
		std::deque<Client *> beta_users;
		std::deque<Client *> invited;
		//add vector client for members
		//add vector for admins 
		//add vector for invited


		channel(std::string name);
		channel(Client *cretor, std::string name, Client *server);
		~channel();

		void set_name(std::string name);
		void set_pass(std::string pass);
		void set_topic(std::string topic);
		void set_MaxUser(int max);

		std::string get_name();
		std::string get_pass();
		std::string get_topic();
		int 		get_maxUsers();
};


#endif
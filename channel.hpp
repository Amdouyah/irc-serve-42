#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <deque>
#include "Client.hpp"

#define ERR_USERNOTINCHANNEL(client, nick, chan) 	("441" + client + " " + nick + " " + chan + " :They aren't on that channel\r\n");
#define ERR_NOTONCHANNEL(client, chan)  			("442" + client + " " + chan +  " :You're not on that channel\r\n");
#define ERR_CHANOPRIVSNEEDED(client, chan)			("482" + client + " " + chan +  " :You're not channel operator\r\n")
class channel{
	public:
		std::string _name;
		bool		invitOnly;

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


		channel(std::string name);
		channel(Client *cretor, std::string name);
		~channel();

		void set_name(std::string name);
		void set_pass(std::string pass);
		void set_topic(std::string topic);
		void set_MaxUser(int max);


		std::string get_name();
		std::string get_pass();
		std::string get_topic();
		int 		get_maxUsers();
		bool 		onChannel(Client *cli);
		bool		isAlpha(Client *cli);

		std::string 	KICK(Client *admin, Client *cli);
		void	rmvUser(Client *cli);

		
};

std::string getUserInfo(Client *cli, bool bil);
 //:amdouyah!amdouyah@88ABE6.25BF1D.D03F86.88C9BD.IP KICK #chann amdouyah :amdouya
#endif
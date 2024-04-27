#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <deque>
#include "Client.hpp"
#include <sstream>


#define ERR_NEEDMOREPARAMS(client, command)				(" 461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_USERNOTINCHANNEL(client, nick, chan) 		(" 441" + client + " " + nick + " " + chan + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, chan)  				(" 442" + client + " " + chan +  " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(client, chan)				(" 482" + client + " " + chan +  " :You're not channel operator\r\n")
#define ERR_USERONCHANNEL(client, nick, chan)			(" 443" + client + " " + nick + " " + chan +  " :is already on channel\r\n")
#define RPL_INVITING(client, nick, chan) 				(" 341 " + client + " " + nick + " " + chan + "\r\n")
#define ERR_NOSUCHCHANNEL(client, chan)	   				(" 403 " + client + " " + chan + " :No such channel\r\n")
#define ERR_UNKNOWNMODE(nick, mode)						(" 472 " + nick + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_NOSUCHNICK(client,nick)						(" 401 "  + client + " " + nick + ":No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(client, chan)	   				(" 403 " + client + " " + chan + " :No such channel\r\n")
#define ERR_PASSWDMISMATCH()							(" 464 :Password incorrect\r\n")
#define ERR_NEEDMOREPARAMS1()							(" 461 :Not enough parameters\r\n")
#define RPL_WELCOME()									(" 001 :Welcome to the Internet Relay Network \r\n")
#define RPL_INVITING(client, nick, chan)				(" 341 " + client + " " + nick + " " + chan + "\r\n")
#define ERR_UNKNOWNCOMMAND(client, command)				(" 421 " + client + " " + command + " :Unknown command\r\n")

class channel{
	private:
		std::string _name;
		bool		invitOnly;

		bool 		has_pass;
		std::string 		passwd;

		std::string Topic;
		bool 		has_topic;
		
		Client 		creat;
		
		bool		limitsuser;
		int			maxUsers;


	public:
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
		void set_Alpha(Client *cli);
		void RmvAlpha(Client *cli);

		bool Invit_Only();
		bool HasTopic();
		bool HasPass();
		bool HasLimit();

		Client 		*getUser(std::string name);
		std::string get_name();
		std::string get_pass();
		std::string get_topic();

		int 		get_maxUsers();
		bool 		onChannel(Client *cli);
		bool		isAlpha(Client *cli);
		bool		isInvit(Client *cli);

		static std::string getUserInfo(Client *cli, bool bil);
		
		//admin howa li ghay kicky

		void		KICK(Client *admin, Client *cli, std::string reason);
		void		INVITE(Client *admin, Client *cli);
		void 		MODE(Client *admin, std::string mode, std::string param);
		void 		PART(Client *cli, std::string reason);

		void 		SetModes(Client *admin, char c, std::string param);
		void 		RemModes(Client *admin, char c, std::string param);
		void		valid_Modes(Client *cli, std::string mode, std::string param);

		void 		addAdmin(Client *cli, std::string param);
		void 		RemAdmin(Client *cli, std::string param);
		void 		changInviteMode(Client *cli, int i);
		void 		changMaxUser(Client *cli, int i, std::string &param);
		void		changKey(Client *cli, int i, std::string &param);
		void		changeTopic(Client *cli, int i);
		
		void		rmvUser(Client *cli);
		static void	setbuffer(std::string msg_to_send, int dest_fd);
		void 		SendToAllClient(std::string buffer);

		
};


#endif
#include "channel.hpp"
#include "Server.hpp"

channel::channel(){
	_name = "default"; 
}
channel::~channel(){
}
channel::channel(std::string name) : _name(name){
}


void channel::set_name(std::string name){
	_name = name;
}
void channel::set_pass(std::string pass){
	passwd = pass;
	has_pass = true;
}
void channel::set_topic(std::string topic){
	Topic = topic;
	has_topic = true;
}
void channel::set_MaxUser(int max){
	limitsuser = true;
	maxUsers = max;
}

std::string channel::get_name(){
	return this->_name;
}
std::string channel::get_pass(){
	return this->passwd;
}
std::string channel::get_topic(){
	return this->Topic;
}
int 		channel::get_maxUsers(){
	return this->maxUsers;
}

std::string getUserInfo(Client *cli, bool bil){
	return (bil ? ":" + cli->nickname + "!" + cli->username + "@" + cli->servername + " " : ":" + cli->servername + " ");
}

bool	channel::onChannel(Client *cli){
	return (std::find(this->beta_users.begin(), this->beta_users.end(), cli) != this->beta_users.end());
}
bool	channel::isAlpha(Client *cli){
	return (std::find(this->alpha_users.begin(), this->alpha_users.end(), cli) != this->alpha_users.end());
}
std::string channel::KICK(Client *admin, Client *cli){
	if (!this->onChannel(admin)){
		//setbuffer
		 return getUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
	}
	else{
		if (!this->isAlpha(admin)){
			//setbuffer
			// Server::setbuffer()
			return getUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->get_name());
		}
		else if(!this->onChannel(cli)){
			return getUserInfo(admin, false) + ERR_USERNOTINCHANNEL(admin->nickname, cli->nickname ,this->get_name());
		}
		else{
			

		}
	}
}
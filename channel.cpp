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
bool	channel::isInvit(Client *cli){
	return (std::find(this->invited.begin(), this->invited.end(), cli) != this->invited.end());
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
			rmvUser(cli);
			return getUserInfo(cli, true) + "KICK" + this->get_name() + " " + cli->nickname + " :" + cli->nickname;
		}
	}
}

void	channel::rmvUser(Client *cli){
	std::deque<Client *>::iterator it = std::find(this->beta_users.begin(), this->beta_users.end(), cli);
	if(it != this->beta_users.end()){
		this->beta_users.erase(it);
	}
	std::deque<Client *>::iterator it2 = std::find(this->alpha_users.begin(), this->alpha_users.end(), cli);
	if(it != this->alpha_users.end()){
		this->alpha_users.erase(it);
	}
	if(this->invitOnly){
		std::deque<Client *>::iterator it3 = std::find(this->invited.begin(), this->invited.end(), cli);
		if(it != this->invited.end())
			this->invited.erase(it);
	}
}

std::string		channel::INVITE(Client *admin, Client *cli){
	if (!this->onChannel(admin)){
		 return getUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
	}
	else if(this->onChannel(cli)){
		// return getUserInfo(cli, false) + ERR_USERONCHANNEL(cli, )
	}
	else{
		if (!this->isAlpha(admin)){
			return getUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->get_name());
		}
	}
}

//2 reply 
//lowla lmol lcommand 341
//tanya lhadak lit invita 
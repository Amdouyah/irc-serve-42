#include "channel.hpp"

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
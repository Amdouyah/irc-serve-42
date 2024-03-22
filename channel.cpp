#include "channel.hpp"

channel::channel(){
	_name = "default"; 
}
channel::~channel(){
}
channel::channel(std::string name) : _name(name){
}
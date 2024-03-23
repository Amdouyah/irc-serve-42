#include "channel.hpp"

channel::channel(){
	_name = "default"; 
}
channel::~channel(){
}
channel::channel(std::string name) {
	this->_name = name;
}
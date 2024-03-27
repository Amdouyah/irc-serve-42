#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include "Client.hpp"
#define deque_itr std::deque<Client *>::iterator
#define deque_player std::vector<players>::iterator

struct players
{
	std::string name;
	int tries;
	int number;
};
class Bot
{

public:
	Bot();
	std::string game(deque_itr it, std::string msg);

private:
	std::string _name;
	std::vector<players> _players;
};



#endif /* ************************************************************* BOT_H */
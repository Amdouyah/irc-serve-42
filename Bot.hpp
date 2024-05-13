#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include "Client.hpp"
#include "channel.hpp"

#define deque_itr std::deque<Client >::iterator
#define deque_player std::vector<players>::iterator
#define deque_cardplayer std::vector<card_players>::iterator

struct players
{
	std::string name;
	int tries;
	int number;
};

struct card_players
{
	std::string name;
	std::vector<std::string> player_cards;
	std::vector<std::string> bot_cards;
};
class Bot
{

public:
	Bot();
	std::string game(deque_itr &it, std::string msg);
	std::string guess_game(deque_itr &it, std::string msg);
	std::string rollDice();
	std::string flipCoin();
	void kick_the_bad_guy(std::string msg, Client *it, std::deque<channel >::iterator &chan);


private:
	std::string _name;
	std::vector<players> _players;
	std::vector<card_players> _card_players;
};



#endif /* ************************************************************* BOT_H */
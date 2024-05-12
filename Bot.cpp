#include "Bot.hpp"
#include "channel.hpp"
#include <sstream>


Bot::Bot()
{
    _name = "jhonny";
}

std::string Bot::game(deque_itr &it, std::string msg)
{
    std::string rtrn;
    if(msg.find(" :play with me") == 0 || (*it).playing_game == true)
        rtrn = this->guess_game(it, msg);
    else if(msg.find(" :flip a coin") == 0)
        rtrn = this->flipCoin();
    else if(msg.find(" :roll a dice") == 0)
        rtrn = this->rollDice();
    else
        return "command are play with me, flip a coin, roll a dice\n";
    return rtrn;
}


std::string Bot::guess_game(deque_itr &it, std::string msg)
{
    bool flag = false;
    deque_player tmp = _players.begin();
    for (; tmp != _players.end(); tmp++)
    {
        if (tmp->name == (*it).nickname)
        {
            flag = true;
            break;
        }
    }
    if (flag == true)
    {
        std::string message =  msg.substr(2);
        std::stringstream ss(message);
        int guess;
        ss>> guess;
        if (guess == tmp->number)
        {
            std::string rtrn = std::string("Congratulations ") + (*it).nickname + std::string(" you guessed the number in ") + std::to_string(tmp->tries) + std::string(" tries \n");
            _players.erase(tmp);
            return rtrn;
        }
        else if (guess > tmp->number)
        {
            tmp->tries++;
            if (tmp->tries < 7)
            {
                return std::string("The number is lower than ") + std::to_string(guess) + std::string("\n");
            }
        }
        else
        {
            tmp->tries++;
            if (tmp->tries < 7)
            {
                return std::string("The number is higher than ") + std::to_string(guess) + std::string("\n");
            }
        }
        if (tmp->tries == 7)
        {
            std::string rtrn = std::string("Sorry ") + (*it).nickname + std::string(" you have used all your tries, the number was ") + std::to_string(tmp->number) + std::string("\n");
            _players.erase(tmp);
            (*it).playing_game = false;
            return rtrn;
        }
    }
    else
    {
        players temp;
        (*it).playing_game = true;
        temp.name = (*it).nickname;
        temp.tries = 0;
        temp.number = rand() % 200;
        _players.push_back(temp);
        std::string rtrn = std::string("Welcome to the game ") + (*it).nickname + std::string(" I have a number between 0 and 200, try to guess it, you have 7 tries.\n i will tell you if the number is higher or lower than the one you guessed, good luck");
        return rtrn;
    }
    return "";
}

std::string Bot::flipCoin()
{
    std::string rtrn = std::string("The coin landed on ");
    if (rand() % 2 == 0)
    {
        rtrn += std::string("heads\n");
    }
    else
    {
        rtrn += std::string("tails\n");
    }
    return rtrn;
}

std::string Bot::rollDice()
{
    return std::string("The dice rolled a ") + std::to_string(rand() % 6 + 1) + std::string("\n");
}


void Bot::kick_the_bad_guy(std::string msg, Client *it, std::deque<channel >::iterator &chan)
{
    if(msg.find("fuck you") != std::string::npos)
    {  
        (*chan).KICK(0, it, "bad boy", true);
    }
}

#include "Bot.hpp"
#include <sstream>


Bot::Bot()
{
    _name = "jhonny";
}

std::string Bot::game(deque_itr it, std::string msg)
{
    bool flag = false;
    deque_player tmp = _players.begin();
    for (; tmp != _players.end(); tmp++)
    {
        if (tmp->name == (*it)->nickname)
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
            std::string rtrn = std::string("Congratulations ") + (*it)->nickname + std::string(" you guessed the number in ") + std::to_string(tmp->tries) + std::string(" tries \n");
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
            std::string rtrn = std::string("Sorry ") + (*it)->nickname + std::string(" you have used all your tries, the number was ") + std::to_string(tmp->number) + std::string("\n");
            _players.erase(tmp);
            return rtrn;
        }
    }
    else
    {
        players temp;
        temp.name = (*it)->nickname;
        temp.tries = 0;
        temp.number = rand() % 200;
        _players.push_back(temp);
        std::string rtrn = std::string("Welcome to the game ") + (*it)->nickname + std::string(" I have a number between 0 and 200, try to guess it, you have 7 tries.\n i will tell you if the number is higher or lower than the one you guessed, good luck");
        return rtrn;
    }
    return "";
}

std::string Bot::card_game(deque_itr it, std::string msg)
{
    bool flag = false;
    deque_cardplayer tmp = _card_players.begin();
    for (; tmp != _card_players.end(); tmp++)
    {
        if (tmp->name == (*it)->nickname)
        {
            flag = true;
            break;
        }
    }
    if (flag == true)
    {
    }
    else
    {
    }
    return "";
}

std::string Bot::flipCoin(deque_itr it, std::string msg)
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

std::string Bot::rollDice(deque_itr it, std::string msg)
{
    return std::string("The dice rolled a ") + std::to_string(rand() % 6 + 1) + std::string("\n");
}

#include "channel.hpp"

channel::channel()
{
	_name = "default";
	invitOnly = false;
	has_pass = false;
	has_topic = false;
	oper = false;
	limitsuser = false;

}
channel::~channel()
{
}
channel::channel(std::string name) : _name(name)
{
}

void channel::set_name(std::string name)
{
	_name = name;
}
void channel::set_pass(std::string pass)
{
	passwd = pass;
	has_pass = true;
}
void channel::set_topic(std::string topic)
{
	Topic = topic;
	has_topic = true;
}
void channel::set_MaxUser(int max)
{
	limitsuser = true;
	maxUsers = max;
}
void channel::set_time(std::string time_){
	this->creation_time = time_;
}

void channel::set_Alpha(Client *cli)
{
	alpha_users.push_back(cli);
}
void channel::RmvAlpha(Client *cli)
{
	std::deque<Client *>::iterator it2 = std::find(this->alpha_users.begin(), this->alpha_users.end(), cli);
	if (it2 != this->alpha_users.end())
	{
		this->alpha_users.erase(it2);
	}
}

std::string channel::get_name()
{
	return this->_name;
}
std::string channel::get_pass()
{
	return this->passwd;
}
std::string channel::get_topic()
{
	return this->Topic;
}
int channel::get_maxUsers()
{
	return this->maxUsers;
}
std::string channel::get_time(){
	return this->creation_time;
}

bool channel::Invit_Only()
{
	return invitOnly;
}
bool channel::HasTopic()
{
	return has_topic;
}
bool channel::HasPass()
{
	return has_pass;
}
bool channel::HasLimit()
{
	return limitsuser;
}

std::string channel::getUserInfo(Client *cli, bool bil)
{
	return (bil ? ":" + cli->nickname + "!" + cli->username + "@" + cli->servername + " " : ":" + cli->servername + " ");
}

bool channel::onChannel(Client *cli)
{
	return (std::find(this->beta_users.begin(), this->beta_users.end(), cli) != this->beta_users.end());
}
bool channel::isAlpha(Client *cli)
{
	return (std::find(this->alpha_users.begin(), this->alpha_users.end(), cli) != this->alpha_users.end());
}
bool channel::isInvit(Client *cli)
{
	return (std::find(this->invited.begin(), this->invited.end(), cli) != this->invited.end());
}

void channel::KICK(Client *admin, Client *cli, std::string reason)
{
	std::string err_msg;
	if (!this->onChannel(admin))
	{
		err_msg = getUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
		setbuffer(err_msg, admin->client_fd);
		return;
	}
	if (!this->isAlpha(admin))
	{
		err_msg = getUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->get_name());
		setbuffer(err_msg, admin->client_fd);
		return;
	}
	if (!this->onChannel(cli))
	{
		err_msg = getUserInfo(cli, false) + ERR_NOTONCHANNEL(cli->nickname, this->get_name());
		setbuffer(err_msg, cli->client_fd);
		return;
	}
	SendToAllClient(getUserInfo(cli, true) + " KICK " + this->get_name() + " " + cli->nickname + " : " + (reason.empty() ? "bad content" : reason) + "\r\n");
	rmvUser(cli);
}

void channel::SendToAllClient(std::string buffer)
{
	for (std::deque<Client *>::iterator it = this->beta_users.begin(); it != this->beta_users.end(); ++it)
	{
		setbuffer(buffer, (*it)->client_fd);
	}
}

void channel::rmvUser(Client *cli)
{
	std::deque<Client *>::iterator it = std::find(this->beta_users.begin(), this->beta_users.end(), cli);
	if (it != this->beta_users.end())
	{
		this->beta_users.erase(it);
	}
	std::deque<Client *>::iterator it2 = std::find(this->alpha_users.begin(), this->alpha_users.end(), cli);
	if (it2 != this->alpha_users.end())
	{
		this->alpha_users.erase(it);
	}
	if (this->invitOnly)
	{
		std::deque<Client *>::iterator it3 = std::find(this->invited.begin(), this->invited.end(), cli);
		if (it3 != this->invited.end())
			this->invited.erase(it);
	}
}

void channel::INVITE(Client *admin, Client *cli)
{
	std::string err_msg;
	if (!this->onChannel(admin))
	{
		err_msg = getUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
		setbuffer(err_msg, admin->client_fd);
		return;
	}
	if (this->onChannel(cli))
	{
		err_msg = getUserInfo(cli, false) + ERR_USERONCHANNEL(admin->nickname, cli->nickname, this->get_name());
		setbuffer(err_msg, cli->client_fd);
		return;
	}
	if (!this->isAlpha(admin))
	{
		err_msg = getUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->get_name());
		setbuffer(err_msg, admin->client_fd);
		return;
	}
	this->invited.push_back(cli);
	setbuffer(getUserInfo(admin, false) + RPL_INVITING(admin->nickname, cli->nickname, this->get_name()), cli->client_fd);
	setbuffer(getUserInfo(admin, false) + RPL_INVITING(admin->nickname, cli->nickname, this->get_name()), admin->client_fd);
}

void channel::setbuffer(std::string msg_to_send, int dest_fd)
{
	int ret;
	if ((ret = send(dest_fd, msg_to_send.c_str(), msg_to_send.size(), 0)) == -1)
		throw std::runtime_error("send failed");
	if (ret != (int)msg_to_send.size())
		throw std::runtime_error("send failed: not all bytes sent");
}

Client *channel::getUser(std::string name)
{
	for (size_t i = 0; i < beta_users.size(); ++i)
	{
		if (name == beta_users[i]->nickname)
			return beta_users[i];
	}
	return NULL;
}
void	channel::TOPIC(Client *cli, std::string topicmsg){
	std::string err_msg;
	std::string rpl_msg;
	if (!this->onChannel(cli))
	{
		err_msg = getUserInfo(cli, false) + ERR_NOTONCHANNEL(cli->nickname, this->get_name());
		setbuffer(err_msg, cli->client_fd);
		return;
	}
	if(has_topic){
		if (!this->isAlpha(cli))
		{
			err_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
			setbuffer(err_msg, cli->client_fd);
			return;
		}
		if(!topicmsg.empty())
		{
			set_topic(topicmsg);
			rpl_msg = getUserInfo(cli, true) + RPL_TOPIC(cli->nickname, this->get_name(), topicmsg);
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
		else{
			rpl_msg = getUserInfo(cli, false) + RPL_NOTOPIC(cli->nickname, this->get_name());
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
	}
	else{
		if(!topicmsg.empty())
		{
			set_topic(topicmsg);
			rpl_msg = getUserInfo(cli, true) + RPL_TOPIC(cli->nickname, this->get_name(), topicmsg);
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
		else{
			rpl_msg = getUserInfo(cli, false) + RPL_NOTOPIC(cli->nickname, this->get_name());
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
	}
}


void channel::MODE(Client *admin, std::string mode, std::string param)
{
	std::string rpl_msg;
	if (!mode.empty())
	{
		if (this->isAlpha(admin))
			valid_Modes(admin, mode, param);
		else
		{
			rpl_msg = getUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->get_name());
			setbuffer(rpl_msg, admin->client_fd);
			return;
		}
	}
	else
	{
		rpl_msg = getUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), modes_);
		rpl_list(admin);
		setbuffer(rpl_msg, admin->client_fd);
		rpl_msg = getUserInfo(admin, true) + RPL_CREATIONTIME(admin->nickname, this->get_name(), this->get_time());
		setbuffer(rpl_msg, admin->client_fd);
	}
}
void channel::valid_Modes(Client *cli, std::string mode, std::string param)
{
	std::string rpl_msg;
	char sign = mode[0];
	size_t i = 0;
	if (sign == '+')
		i++;
	if (sign != '-')
	{
		for (; i < mode.size(); ++i)
		{
			if (mode[i] == 't' || mode[i] == 'k' || mode[i] == 'i' || mode[i] == 'l' || mode[i] == 'o'){
				SetModes(cli, mode[i], param);
				check_modes();
			}
			else
			{
				rpl_msg = getUserInfo(cli, false) + ERR_UNKNOWNMODE(cli->nickname, mode[i]);
				setbuffer(rpl_msg, cli->client_fd);
			}
		}
	}
	else
	{
		for (i = 1; i < mode.size(); ++i)
		{
			if (mode[i] == 't' || mode[i] == 'k' || mode[i] == 'i' || mode[i] == 'l' || mode[i] == 'o'){
				RemModes(cli, mode[i], param);
				check_modes();
			}
			else
			{
				rpl_msg = getUserInfo(cli, false) + ERR_UNKNOWNMODE(cli->nickname, mode[i]);
				setbuffer(rpl_msg, cli->client_fd);
			}
		}
	}
}

void channel::SetModes(Client *admin, char c, std::string param)
{
	if (c == 'i')
		this->changInviteMode(admin, 1);
	else if (c == 'k')
		changKey(admin, 1, param);
	else if (c == 't')
		changeTopic(admin, 1);
	else if (c == 'o')
		addAdmin(admin, param);
	else if (c == 'l')
		changMaxUser(admin, 1, param);
}
void channel::RemModes(Client *admin, char c, std::string param)
{
	if (c == 'i')
		this->changInviteMode(admin, 0);
	else if (c == 'k')
		changKey(admin, 0, param);
	else if (c == 't')
		changeTopic(admin, 0);
	else if (c == 'o')
		RemAdmin(admin, param);
	else if (c == 'l')
		changMaxUser(admin, 0, param);
}

void channel::PART(Client *cli, std::string reason)
{
	std::string err_msg;
	if (!onChannel(cli))
	{
		err_msg = getUserInfo(cli, false) + ERR_NOTONCHANNEL(cli->nickname, this->get_name());
		setbuffer(err_msg, cli->client_fd);
	}
	else
	{
		SendToAllClient(getUserInfo(cli, true) + " PART " + this->get_name() + " " + reason + "\r\n");
		this->rmvUser(cli);
	}
}

static int convertToInt(const std::string &str)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isdigit(str[i]))
			return 0;
	}

	std::istringstream input(str);
	int num;
	input >> num;
	if (num <= 0)
		return 0;
	return num;
}

void	channel::check_modes(){
	std::string addModes;
	if(has_pass)
		addModes += "k";
	if(invitOnly)
		addModes += "i";
	if(limitsuser)
		addModes += "l";
	if(oper)
		addModes += "o";
	if(has_topic)
		addModes += "t";
	this->modes_ = addModes;
}


void channel::addAdmin(Client *cli, std::string param)
{
	this->oper = true;
	std::string rpl_msg;
	if (!isAlpha(cli))
	{
		rpl_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	if (param.empty())
	{
		rpl_msg = getUserInfo(cli, false) + ERR_NEEDMOREPARAMS(cli->nickname, " MODE " + " +o " + param);
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	Client *newopp = getUser(param);
	if (newopp)
	{
		set_Alpha(newopp);
		SendToAllClient(getUserInfo(newopp, true) + RPL_CHANNELMODEIS(newopp->nickname, this->_name, "+o " + newopp->nickname));
	}
	else
	{
		rpl_msg = getUserInfo(newopp, false) + ERR_USERNOTINCHANNEL(cli->nickname, newopp->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
	}
}
void channel::RemAdmin(Client *cli, std::string param)
{
	this->oper = false;
	std::string rpl_msg;
	if (!isAlpha(cli))
	{
		rpl_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	Client *newopp = getUser(param);
	if (newopp)
	{
		RmvAlpha(newopp);
		SendToAllClient(getUserInfo(newopp, true) + RPL_CHANNELMODEIS(newopp->nickname, this->_name, " -o " + newopp->nickname));
	}
	else
	{
		rpl_msg = getUserInfo(newopp, false) + ERR_USERNOTINCHANNEL(cli->nickname, newopp->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
	}
}
void channel::changInviteMode(Client *cli, int i)
{
	std::string rpl_msg;
	if (!isAlpha(cli))
	{
		rpl_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	if (i == 1)
	{
		invitOnly = true;
		SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "+i"));
	}
	else
	{
		invitOnly = false;
		SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "-i"));
	}
}
void channel::changMaxUser(Client *cli, int i, std::string &param)
{
	std::string rpl_msg;
	int max;
	if (!isAlpha(cli))
	{
		rpl_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	if (i == 1)
	{
		if (param.empty())
		{
			rpl_msg = getUserInfo(cli, false) + ERR_NEEDMOREPARAMS(cli->nickname,  "MODE" + " +l ");
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
		max = convertToInt(param);
		if (max <= 0)
		{
			rpl_msg = getUserInfo(cli, false) + ERR_NEEDMOREPARAMS(cli->nickname, " MODE" + " +l ");
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
		else
		{
			limitsuser = true;
			set_MaxUser(max);
			SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "+l"));
		}
	}
	else
	{
		limitsuser = false;
		SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "-l"));
	}
}

void channel::who(Client *cli, Client *user)
{
	std::string rpl_msg;

	rpl_msg += ":" + cli->servername + " 352 " + cli->nickname + " " + this->get_name() + " " + user->username + " " + user->username + " " + user->client_ip + " " + user->nickname + " H";
	if (isAlpha(user))
		rpl_msg += "@";
	rpl_msg += ":0 realname\r\n";
	setbuffer(rpl_msg, cli->client_fd);
}
void channel::rpl_who(Client *cli)
{
	for (size_t i = 0; i < beta_users.size(); ++i)
	{
		who(cli, beta_users[i]);
	}
	setbuffer(getUserInfo(cli, false) + RPL_ENDOFWHOIS(cli->nickname, this->get_name()), cli->client_fd);
}

void channel::changKey(Client *cli, int i, std::string &param)
{
	std::string rpl_msg;
	if (!isAlpha(cli))
	{
		rpl_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	if (i == 1)
	{
		if (param.empty())
		{
			rpl_msg = getUserInfo(cli, false) + ERR_NEEDMOREPARAMS(cli->nickname, " MODE" + " +k");
			setbuffer(rpl_msg, cli->client_fd);
			return;
		}
		else
		{
			set_pass(param);
			has_pass = true;
			SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "+k"));
		}
	}
	else
	{
		has_pass = false;
		SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "-k"));
	}
}
void channel::changeTopic(Client *cli, int i)
{
	std::string rpl_msg;
	if (!isAlpha(cli))
	{
		rpl_msg = getUserInfo(cli, false) + ERR_CHANOPRIVSNEEDED(cli->nickname, this->get_name());
		setbuffer(rpl_msg, cli->client_fd);
		return;
	}
	if (i == 1)
	{
		has_topic = true;
		SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "+t"));
	}
	else
	{
		has_topic = false;
		SendToAllClient(getUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->nickname, this->_name, "-t"));
	}
}


void	channel::rpl_list(Client *cli){
	std::string rpl_msg;

	rpl_msg += getUserInfo(cli, false) + " 353 " + cli->nickname + " = " + this->get_name() + " :";
	for(size_t i = 0; i < beta_users.size() ; ++i){
		if(isAlpha(beta_users[i]))
			rpl_msg += "@";
		rpl_msg += beta_users[i]->nickname + " ";
	}
	rpl_msg += "\r\n";
	setbuffer(rpl_msg, cli->client_fd);
	setbuffer(getUserInfo(cli, false) + RPL_ENDOFWHOIS(cli->nickname, this->get_name()), cli->client_fd);
}

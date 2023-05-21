#include "Database.hpp"
#include "Udata.hpp"
#include "User.hpp"
#include <string>
#include <sys/_types/_ct_rune_t.h>

#include "debug.hpp"

std::vector<User> &Database::get_user_list(void)
{
	return user_list_;
}

void Database::delete_error_user(const uintptr_t &ident)
{
	if (!is_user(ident))
	{
		return;
	}
	User &cur_usr = select_user(ident);
	if (is_user_in_channel(cur_usr))
	{
		Channel &cur_chan = select_channel(cur_usr);

		std::vector<User> &users = cur_chan.get_users();
		const int user_size = users.size();
		cur_chan.delete_user(cur_usr);
		if (user_size == 1)
			delete_channel(cur_chan.get_name());
	}
	user_list_.erase(remove(user_list_.begin(), user_list_.end(), cur_usr), user_list_.end());
}

Event Database::valid_user_checker_(const uintptr_t &ident, const std::string &command_type)
{
	Event ret;

	ret.first = ident;
	if (!is_user(ident))
	{
		return Sender::password_incorrect_464(ident);
	}
	User &cur_user = select_user(ident);
	if (!(cur_user.flag_ & F_PASS))
	{
		return Sender::password_incorrect_464(ident);
	}
	if (command_type == "NICK" || command_type == "USER")
	{
		return ret;
	}
	if (!(cur_user.flag_ & F_NICK))
	{
		return Sender::command_not_registered_451(ident, command_type);
	}
	if (!(cur_user.flag_ & F_USER))
	{
		return Sender::command_not_registered_451(cur_user, command_type);
	}
	return ret;
}

User &Database::select_user(const uintptr_t &ident)
{
	std::vector<User>::iterator it;

	for (it = user_list_.begin(); it != user_list_.end(); it++)
	{
		if (it->client_sock_ == ident)
		{
			return (*it);
		}
	}
	return (*it);
}

User &Database::select_user(const std::string &nickname)
{
	std::vector<User>::iterator it;

	for (it = user_list_.begin(); it != user_list_.end(); it++)
	{
		if (it->nickname_ == nickname)
		{
			return (*it);
		}
	}
	return (*it);
}

bool Database::is_user(const uintptr_t &ident)
{
	for (std::vector<User>::iterator it = user_list_.begin(); it != user_list_.end(); it++)
	{
		if (it->client_sock_ == ident)
		{
			return (true);
		}
	}
	return (false);
}
bool Database::is_user(const std::string &nickname)
{
	for (std::vector<User>::iterator it = user_list_.begin(); it != user_list_.end(); it++)
	{
		if (it->nickname_ == nickname)
		{
			return (true);
		}
	}
	return (false);
}

bool Database::does_has_nickname(const uintptr_t &ident)
{
	try
	{
		User &tmp = select_user(ident);
		if (tmp.nickname_.empty())
			return (false);
	}
	catch (const std::exception &)
	{
		return (false);
	}
	return (true);
}

bool Database::does_has_username(const uintptr_t &ident)
{
	try
	{
		User &tmp = select_user(ident);
		if (tmp.username_.empty())
			return (false);
	}
	catch (const std::exception &)
	{
		return (false);
	}
	return (true);
}

void Database::delete_user(User &leaver)
{
	int idx = 0;
	std::vector<User>::iterator it;

	for (it = user_list_.begin(); it != user_list_.end(); it++)
	{
		if (*it == leaver)
		{
			user_list_.erase(user_list_.begin() + idx);
			break;
		}
		idx++;
	}
}

bool Database::is_valid_nick(std::string &new_nick)
{
	if (!isalpha(static_cast<int>(new_nick[0])) && static_cast<int>(new_nick[0]) != '_')
		return false;
	for (std::size_t i(1); i < new_nick.size(); i++)
	{
		if (!isalnum(static_cast<int>(new_nick[i])) && static_cast<int>(new_nick[i]) != '_')
		{
			return false;
		}
	}
	return true;
}

Udata Database::run_mode(const uintptr_t &ident, t_mode &mode)
{
	if (mode.mode_type == PLUS_I)
		return command_mode_i_on(ident, mode);
	else if (mode.mode_type == MINUS_I)
		return command_mode_i_off(ident, mode);
	else if (mode.mode_type == PLUS_K)
		return (command_mode_k_on(ident, mode));
	else if (mode.mode_type == MINUS_K)
		return (command_mode_k_off(ident, mode));
	else if (mode.mode_type == PLUS_O)
		return command_mode_o_on(ident, mode);
	else if (mode.mode_type == MINUS_O)
		return command_mode_o_off(ident, mode);
	else if (mode.mode_type == PLUS_T)
		return command_mode_t_on(ident, mode);
	else if (mode.mode_type == MINUS_T)
		return command_mode_t_off(ident, mode);
	else if (mode.mode_type == PLUS_L)
		return command_mode_l_on(ident, mode);
	else
		return command_mode_l_off(ident, mode);
}

Udata Database::command_mode(const uintptr_t &ident, t_mode &mode)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "MODE");

	if (tmp.second.size())
	{
		ret.insert(tmp);
	}
	else if (is_channel(mode.target) == false)
	{
		tmp = Sender::no_channel_message(select_user(ident), mode.target);
		ret.insert(tmp);
		return ret;
	}
	else
		ret = run_mode(ident, mode);
	return ret;
}

Udata Database::command_invite(const uintptr_t &ident, std::string &user, std::string &chan_name)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "INVITE");

	if (tmp.second.size())
	{
		ret.insert(tmp);
	}
	else if (is_channel(chan_name) == false)
	{
		Event tmp = Sender::no_channel_message(select_user(ident), chan_name);
		ret.insert(tmp);
	}
	else if (is_user(user) == false)
	{
		Event tmp = Sender::invite_no_user_message(select_user(ident), user);
		ret.insert(tmp);
	}
	else if (is_user_in_channel(select_user(user)) == true) // 이미 이 방에 있는 유저라면
	{
		Event tmp = Sender::already_in_channel_message(select_user(ident), chan_name);
		ret.insert(tmp);
	}
	else
	{
		// if (is_user_in_channel(cur_usr))
		// {
		// 	Channel &cur_chan = select_channel(cur_usr);
		// 	ret = quit_channel(cur_usr, cur_chan.get_name(), msg);
		// }
		// 	else
		// 	{
		// 		tmp = Sender::quit_leaver_message(cur_usr, msg);
		// 		ret.insert(tmp);
		// 	}

		// 127.000.000.001.06667-127.000.000.001.59616: :irc.local 341 A C :#6 // A에게 보내는 메세지 (초대한 자)
		// 127.000.000.001.06667-127.000.000.001.46928: :irc.local NOTICE #6 :*** A invited C into the channel // C에게 보내는 메시지(초대받은자)

		User &invitor = select_user(ident);
		User &invited_user = select_user(user);
		Channel &cur_channel = select_channel(chan_name);
		cur_channel.invite_user(invited_user.client_sock_);
		ret = cur_channel.send_all(invitor, invited_user, chan_name, INVITE);
		tmp.first = invitor.client_sock_;
		tmp.second.clear();
		ret.insert(tmp);
	}
	return ret;
}

Event Database::command_pass(const uintptr_t &ident)
{
	Event tmp;

	tmp.first = ident;
	if (!is_user(ident))
	{
		User tmp_user;
		tmp_user.client_sock_ = ident;
		tmp_user.flag_ |= F_PASS;
		user_list_.push_back(tmp_user);
	}
	debug::showUsers(user_list_);
	return tmp;
}

Udata Database::command_nick(const uintptr_t &ident, std::string &new_nick)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "NICK");

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	User &cur_usr = select_user(ident);
	if (!is_valid_nick(new_nick))
	{
		if (cur_usr.nickname_.empty())
			tmp = Sender::nick_wrong_message(ident, new_nick);
		else
		{
			User &you_usr = select_user(cur_usr.nickname_);
			tmp = Sender::nick_wrong_message(you_usr, new_nick);
		}
		ret.insert(tmp);
		return ret;
	}
	if (is_user(new_nick))
	{
		User &you_usr = select_user(new_nick);
		if (ident == you_usr.client_sock_)
			return ret;
		if (you_usr.username_.size())
		{
			if (cur_usr.nickname_.empty())
				tmp = Sender::nick_error_message(ident, new_nick);
			else
				tmp = Sender::nick_error_message(cur_usr, new_nick);
			ret.insert(tmp);
			return ret;
		}
		tmp = Sender::nick_error_message2(you_usr, new_nick);
		ret.insert(tmp);
		tmp = Sender::nick_steal_message(cur_usr, new_nick);
		ret.insert(tmp);
		std::cerr << "before -> you_usr flag : " << std::bitset<4>(you_usr.flag_) << '\n';
		you_usr.nickname_.clear();
		you_usr.flag_ &= ~F_NICK;
		std::cerr << "after  -> you_usr flag : " << std::bitset<4>(you_usr.flag_) << '\n';
	}
	if (!(cur_usr.flag_ & F_NICK))
	{
		cur_usr.nickname_ = new_nick;
		cur_usr.flag_ |= F_NICK;
		if (cur_usr.flag_ & F_USER)
		{
			tmp = Sender::welcome_message_connect(cur_usr);
			ret.insert(tmp);
		}
	}
	else
	{
		if (is_user_in_channel(cur_usr))
		{
			ret = nick_channel(cur_usr, new_nick);
		}
		tmp = Sender::nick_well_message(cur_usr, cur_usr, new_nick);
		cur_usr.nickname_ = new_nick;
		ret.insert(tmp);
	}
	debug::showUsers(user_list_);
	return ret;
}

Event Database::command_user(const uintptr_t &ident, const std::string &username, const std::string &mode, const std::string &unused, const std::string &realname)
{
	Event ret = valid_user_checker_(ident, "USER");

	if (ret.second.size())
	{
		return ret;
	}
	User &cur_usr = select_user(ident);
	cur_usr.input_user(username, mode, unused, realname);
	if (!(cur_usr.flag_ & F_USER))
	{
		cur_usr.flag_ |= F_USER;
		if (cur_usr.flag_ & F_NICK)
			ret = Sender::welcome_message_connect(cur_usr);
		else
			ret = Sender::user_302_message(cur_usr);
	}
	else
		ret = Sender::user_302_message(cur_usr);
	debug::showUsers(user_list_);
	return ret;
}

Event Database::command_pong(const uintptr_t &ident, const std::string &target, const std::string &msg)
{
	Event ret = valid_user_checker_(ident, "PING");

	if (ret.second.size())
	{
		return ret;
	}
	User &cur_user = select_user(ident);
	if (msg.at(0) == ':')
	{
		ret = Sender::command_no_origin_specified_409(cur_user, "PING");
		return ret;
	}
	ret = Sender::pong(ident, target, msg);
	return ret;
}

Udata Database::command_join(const uintptr_t &ident, const std::string &chan_name, const std::string &tmp_password)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "JOIN");

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	else if (is_user(ident))
	{
		User &cur_usr = select_user(ident);
		if (chan_name.empty())
		{
			if (!is_user(ident))
			{
				tmp = Sender::command_empty_argument_461(ident, "JOIN");
				ret.insert(tmp);
			}
			else
			{
				tmp = Sender::command_empty_argument_461(cur_usr, "JOIN");
				ret.insert(tmp);
			}
		}
		else if (chan_name.at(0) != '#')
		{
			tmp = Sender::join_invalid_channel_name_message(cur_usr, chan_name);
			ret.insert(tmp);
		}
		else if (chan_name.find(',') != std::string::npos)
		{
			tmp = Sender::join_invalid_channel_name_message(cur_usr, chan_name);
			ret.insert(tmp);
		}
		else
		{
			ret = join_channel(cur_usr, chan_name, tmp_password);
		}
	}
	return ret;
}

Udata Database::command_part(const uintptr_t &ident, std::string &chan_name, const std::string &msg)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "PART");

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	User &cur_usr = select_user(ident);
	ret = part_channel(cur_usr, chan_name, msg);
	return ret;
}

Udata Database::command_quit(const uintptr_t &ident, const std::string &msg)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "QUIT");

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	else if (is_user(ident))
	{
		User &cur_usr = select_user(ident);

		if (is_user_in_channel(cur_usr))
		{
			Channel &cur_chan = select_channel(cur_usr);
			ret = quit_channel(cur_usr, cur_chan.get_name(), msg);
		}
		else
		{
			tmp = Sender::quit_leaver_message(cur_usr, msg);
			ret.insert(tmp);
		}
		user_list_.erase(remove(user_list_.begin(), user_list_.end(), cur_usr), user_list_.end());
	}
	return ret;
}

Udata Database::command_privmsg(const uintptr_t &ident, const std::string &target_name, const std::string &msg)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "PRIVMSG");

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	if (is_user(ident))
	{
		User &cur_usr = select_user(ident);

		tmp.first = ident;
		if (target_name.at(0) == '#')
		{
			ret = channel_msg(cur_usr, target_name, msg);
		}
		else
		{
			if (target_name == "BOT")
			{
				tmp = bot_privmsg(cur_usr, msg);
				ret.insert(tmp);
			}
			else if (is_user(target_name))
			{
				User &tar_usr = select_user(target_name);
				tmp = Sender::privmsg_p2p_message(cur_usr, tar_usr, msg);
				ret.insert(tmp);
				tmp.first = cur_usr.client_sock_;
				tmp.second.clear();
				ret.insert(tmp);
			}
			else
			{
				tmp = Sender::privmsg_no_user_error_message(cur_usr, target_name);
				ret.insert(tmp);
			}
		}
	}
	return ret;
}

Udata Database::command_notice(const uintptr_t &ident, const std::string &target_name, const std::string &msg)
{
	Udata ret;
	Event tmp = valid_user_checker_(ident, "NOTICE");

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	if (target_name == "BOT")
	{
		ret.insert(tmp);
		return ret;
	}
	User &cur_usr = select_user(ident);
	if (target_name.at(0) == '#')
	{
		ret = notice_channel(cur_usr, target_name, msg);
	}
	else
	{
		// if (is_user(target_name))
		// {
		// 	User &tar_usr = select_user(target_name);

		// 	tmp = Sender::notice_p2p_message(cur_usr, tar_usr, msg);
		// 	ret.insert(tmp);
		// }
		if (is_user(target_name))
		{
			User &tar_usr = select_user(target_name);
			tmp = Sender::privmsg_p2p_message(cur_usr, tar_usr, msg);
			ret.insert(tmp);
			tmp.first = cur_usr.client_sock_;
			tmp.second.clear();
			ret.insert(tmp);
		}
		else
		{
			tmp = Sender::notice_no_nick_message(cur_usr, cur_usr);
			ret.insert(tmp);
		}
	}
	return ret;
}

Udata Database::command_kick(const uintptr_t &ident, const std::string &target_name, std::string &chan_name, std::string &msg)
{
	Event tmp = valid_user_checker_(ident, "KICK");
	Udata ret;

	if (tmp.second.size())
	{
		ret.insert(tmp);
		return ret;
	}
	if (is_user(ident))
	{
		User &kicker = select_user(ident);
		if (is_user(target_name))
		{
			User &target = select_user(target_name);
			ret = kick_channel(kicker, target, chan_name, msg);
		}
		else
		{
			tmp = Sender::no_user_message(kicker, target_name);
			ret.insert(tmp);
		}
	}
	return ret;
}

void Database::bot_maker(const std::string &name)
{

	User tmp_usr;

	tmp_usr.nickname_ = name;
	tmp_usr.input_user("Dummy", "Dummy", "localhost", "Dummy");
	user_list_.push_back(tmp_usr);
}
Event Database::bot_privmsg(User &cur_usr, const std::string &msg)
{
	Event tmp;
	std::string bot_msg;

	if (msg == "!command")
	{
		bot_msg = "NICK, USER, PING, JOIN, QUIT, PRIVMSG, KICK, PART, TOPIC, NOTICE";
	}
	else if (msg == "!channel")
	{
		if (channel_list_.empty())
		{
			bot_msg = "NO CHANNEL IN THIS SERVER!";
		}
		else
		{
			bot_msg = "● [CHANNEL LIST] : ";
			for (std::size_t i(0); i < channel_list_.size(); ++i)
			{
				bot_msg += std::to_string(i) + ". " + channel_list_[i].get_name() + " : " + channel_list_[i].get_topic() + ((i == (channel_list_.size() - 1)) ? "" : ", ");
			}
		}
	}
	else if (msg == "!user")
	{
		bot_msg = "● [USER LIST] : ";
		std::size_t n(1);
		for (std::size_t i(1); i < user_list_.size(); ++i)
		{
			if ((user_list_[i].flag_ & F_PASS) && (user_list_[i].flag_ & F_NICK))
				bot_msg += std::to_string(n++) + ". " + user_list_[i].nickname_ + ((i == (user_list_.size() - 1)) ? "" : ", ");
		}
	}
	else
	{
		bot_msg = "THAT IS NOT MY COMMAND. YOU CAN USE : '!commanmd' & '!channel' & !user.";
	}
	tmp = Sender::privmsg_bot_message(cur_usr, bot_msg);
	return tmp;
}

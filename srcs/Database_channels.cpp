#include "Channel.hpp"
#include "Database.hpp"
#include "Udata.hpp"
#include "User.hpp"
#include "color.hpp"
#include <sys/_types/_size_t.h>
#include <iostream>
#include <sstream>
//
#include <bitset>

bool Database::is_channel(std::string &chan_name)
{
	std::vector<Channel>::iterator it;

	for (it = channel_list_.begin(); it != channel_list_.end(); ++it)
	{
		if (it->get_name() == chan_name)
		{
			return true;
		}
	}
	return false;
}

bool Database::is_user_in_channel(User &leaver)
{
	std::vector<Channel>::iterator it = channel_list_.begin();

	for (; it != channel_list_.end(); ++it)
	{
		if (it->is_user(leaver))
		{
			return true;
		}
	}
	return false;
}

Channel &Database::create_channel(User &joiner, std::string &chan_name, std::string chan_access)
{
	Channel tmp;

	tmp.set_channel_name(chan_name);
	tmp.add_user(joiner);
	tmp.set_host(joiner);
	tmp.set_access(chan_access);
	channel_list_.push_back(tmp);
	return channel_list_.back();
}

void Database::delete_channel(std::string &chan_name)
{
	Channel tmp;

	tmp.set_channel_name(chan_name);

	std::vector<Channel>::iterator it = std::find(channel_list_.begin(),
																								channel_list_.end(), tmp);
	std::size_t size = std::distance(this->channel_list_.begin(), it);
	this->channel_list_.erase(this->channel_list_.begin() + size);
}

Channel &Database::select_channel(std::string &chan_name)
{
	std::vector<Channel>::iterator it = channel_list_.begin();

	for (; it != channel_list_.end(); ++it)
	{
		if (it->get_name() == chan_name)
		{
			return *it;
		}
	}
	return *it;
}

Channel &Database::select_channel(const std::string &chan_name)
{
	std::vector<Channel>::iterator it = channel_list_.begin();

	for (; it != channel_list_.end(); ++it)
	{
		if (it->get_name() == chan_name)
		{
			return *it;
		}
	}
	return *it;
}

Channel &Database::select_channel(User &connector)
{
	std::vector<Channel>::iterator it = channel_list_.begin();

	for (; it != channel_list_.end(); ++it)
	{
		if (it->is_user(connector))
		{
			return *it;
		}
	}
	return *it;
}

Udata Database::join_channel(User &joiner, const std::string &tmp_chan_name, const std::string &tmp_password)
{
	Udata ret;
	Event tmp;
	std::string chan_name(tmp_chan_name);
	Channel &cur_chan = select_channel(tmp_chan_name);

	if (is_user_in_channel(joiner))
	{
		Event tmp2;
		tmp = Sender::join_message(joiner, joiner, chan_name);
		tmp2 = Sender::part_message(joiner, joiner, chan_name, "invalid : No Double join");
		tmp.second += tmp2.second;
		ret.insert(tmp);
		return ret;
	}
	if (is_channel(chan_name) == false)
	{
		Channel &chan = create_channel(joiner, chan_name, "=");
		tmp = Sender::join_message(joiner, joiner, chan_name);
		ret.insert(tmp);
		Udata_iter it = ret.find(joiner.client_sock_);
		it->second += Sender::join_353_message(joiner, chan.get_name(), chan.get_access(), "@" + joiner.nickname_);
		it->second += Sender::join_366_message(joiner, chan.get_name());
	}
	else if ((cur_chan.channel_flag_ & F_INVITE_ONLY) && !cur_chan.has_invitation(joiner.client_sock_))
	{
		tmp = Sender::cannot_join_message(joiner, chan_name); // cannot_join_messeage_invite
		ret.insert(tmp);
	}
	else if ((cur_chan.channel_flag_ & F_KEY_NEEDED) && !cur_chan.check_password(cur_chan, tmp_password))
	{
		tmp = Sender::cannot_join_message_key(joiner, chan_name); // connot_join_message_key
		ret.insert(tmp);
	}
	else
	{
		std::cout << "bitset: " << std::bitset<3>(cur_chan.channel_flag_) << "\n";
		std::cout << ", has invitation: " << cur_chan.has_invitation(joiner.client_sock_) << "\n";
		Channel &chan = select_channel(chan_name);
		chan.add_user(joiner);
		const std::string &chan_user_list(chan.get_user_list_str());
		ret = chan.send_all(joiner, joiner, "Join \"" + chan_name + "\" channel, " + joiner.nickname_, JOIN);
		Udata_iter it = ret.find(joiner.client_sock_);
		it->second += Sender::join_353_message(joiner, chan.get_name(), chan.get_access(), chan_user_list);
		it->second += Sender::join_366_message(joiner, chan.get_name());
	}
	return ret;
}

Udata Database::quit_channel(User &leaver, std::string &chan_name, const std::string &msg_)
{
	Event tmp;
	Udata ret;
	std::string msg(msg_);

	if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(leaver, chan_name);
		ret.insert(tmp);
		return ret;
	}
	Channel &chan = select_channel(chan_name);
	if (chan.is_user(leaver) == 0)
	{
		tmp = Sender::no_user_message(leaver, leaver.nickname_);
		return ret;
	}
	std::vector<User> &users = chan.get_users();
	const int user_size = users.size();
	ret = chan.send_all(leaver, leaver, msg, QUIT);
	chan.delete_user(leaver);

	if (user_size == 1)
		delete_channel(chan_name);
	return ret;
}

Udata Database::part_channel(User &leaver, std::string &chan_name, const std::string &msg_)
{
	Event tmp;
	Udata ret;
	std::string msg(msg_);

	if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(leaver, chan_name);
		ret.insert(tmp);
		return ret;
	}
	Channel &chan = select_channel(chan_name);
	if (chan.is_user(leaver) == 0)
	{
		tmp = Sender::no_user_message(leaver, leaver.nickname_);
		return ret;
	}
	std::vector<User> users = chan.get_users();
	int user_size = users.size();
	ret = chan.send_all(leaver, leaver, msg, PART);
	chan.delete_user(leaver);
	if (user_size == 1)
		delete_channel(chan_name);
	return ret;
}

Udata Database::channel_msg(User &sender, std::string chan_name, const std::string &msg)
{
	Udata ret;
	Event tmp;

	if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(sender, chan_name);
		ret.insert(tmp);
		return ret;
	}
	Channel &channel = select_channel(chan_name);
	if (channel.is_user(sender) == false)
	{
		tmp = Sender::no_user_message(sender, sender.nickname_);
		ret.insert(tmp);
		return ret;
	}
	ret = channel.send_all(sender, sender, msg, PRIV);
	return ret;
}

Udata Database::notice_channel(User &sender, std::string chan_name, const std::string &msg)
{
	Udata ret;
	Event tmp;

	if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(sender, chan_name);
		ret.insert(tmp);
		return ret;
	}
	Channel &channel = select_channel(chan_name);
	if (channel.is_user(sender) == false)
	{
		tmp = Sender::no_user_message(sender, sender.nickname_);
		ret.insert(tmp);
		return ret;
	}
	ret = channel.send_all(sender, sender, msg, NOTICE);
	return ret;
}

Udata Database::kick_channel(User &host, User &target, std::string &chan_name, std::string &msg)
{
	Udata ret;
	Event tmp;

	if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(host, chan_name);
		ret.insert(tmp);
		return ret;
	}
	Channel &channel = select_channel(chan_name);
	if (channel.is_host(host))
	{
		if (channel.is_user(target) == true)
		{
			ret = channel.send_all(host, target, msg, KICK);
			channel.delete_user(target);
		}
		else
		{
			tmp = Sender::kick_error_no_user_message(host, host.nickname_, target.nickname_, chan_name);
			ret.insert(tmp);
		}
	}
	else
	{
		tmp = Sender::kick_error_not_op_message(host, host.nickname_, chan_name);
		ret.insert(tmp);
	}
	return ret;
}

/**		nick_channel   **/
/**		@brief NICK 명령어를 채널 안에서 한 경우   **/
/**		@brief 변경했다고 채널 모두에게 send_all, 채널 객체 nick 변경  **/

Udata Database::nick_channel(User &nicker, std::string &send_msg)
{
	Udata ret;
	Event tmp;
	User dummy_user;

	Channel &channel = select_channel(nicker);
	ret = channel.send_all(nicker, dummy_user, send_msg, NICK);
	channel.change_nick(nicker, send_msg);
	return ret;
}

Udata Database::set_topic(User &sender, std::string &chan_name, std::string &topic)
{
	Udata ret;
	Event tmp = valid_user_checker_(sender.client_sock_, "INVITE");
	Channel &channel = select_channel(chan_name);

	if (tmp.second.size())
	{
		ret.insert(tmp);
	}
	else if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(sender, chan_name);
		ret.insert(tmp);
		return ret;
	}
	if ((channel.channel_flag_ & F_TOPIC_OWNERSHIP) && !(channel.is_host(sender)))
	{
		tmp = Sender::topic_access_error(sender, chan_name);
		ret.insert(tmp);
	}
	else
	{
		std::string topic_msg = "Topic was changed to " + topic;
		channel.set_topic(topic);
		ret = channel.send_all(sender, sender, topic_msg, TOPIC);
	}
	return ret;
}

Udata Database::command_mode_i_on(const uintptr_t &ident, t_mode &mode)
{
	Udata ret;
	Event tmp;

	Channel &tmp_channel = select_channel(mode.target);
	User &host = select_user(ident);
	if (tmp_channel.is_host(host))
	{
		tmp_channel.set_flag(tmp_channel, mode);
		ret = tmp_channel.send_all(host, host, "+i", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_i_off(const uintptr_t &ident, t_mode &mode)
{
	Udata ret;
	Event tmp;

	Channel &tmp_channel = select_channel(mode.target);
	User &host = select_user(ident);
	if (tmp_channel.is_host(host))
	{
		tmp_channel.set_flag(tmp_channel, mode);
		ret = tmp_channel.send_all(host, host, "-i", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_k_on(const uintptr_t &ident, t_mode &mode)
{
	Udata ret;
	Event tmp;
	Channel &tmp_channel = select_channel(mode.target);
	User &host = select_user(ident);
	if (tmp_channel.is_host(host))
	{
		tmp_channel.set_password(tmp_channel, mode);
		tmp_channel.set_flag(tmp_channel, mode);
		ret = tmp_channel.send_all(host, host, "+k", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_k_off(const uintptr_t &ident, t_mode &mode)
{
	Udata ret;
	Event tmp;

	Channel &tmp_channel = select_channel(mode.target);
	User &host = select_user(ident);
	if (tmp_channel.is_host(host))
	{
		tmp_channel.set_flag(tmp_channel, mode);
		ret = tmp_channel.send_all(host, host, "-k", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_o_on(const uintptr_t &socket, t_mode mode)
{
	Udata ret;
	Event tmp;

	if (mode.param.length() == 0)
	{
		tmp = Sender::mode_syntax_error(select_user(socket), mode.target, mode.option, "op", "nick");
		ret.insert(tmp);
		return ret;
	}
	else if (!is_user(mode.param))
	{
		tmp = Sender::mode_no_user_message(select_user(socket), mode.param);
		ret.insert(tmp);
		return ret;
	}

	Channel &target_channel = select_channel(mode.target);
	User &host_user = select_user(socket);
	if (target_channel.is_host(host_user))
	{
		if (!target_channel.is_host(select_user(mode.param)))
		{
			target_channel.set_host(select_user(mode.param));
		}
		ret = target_channel.send_all(host_user, host_user, "+o", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host_user, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_o_off(const uintptr_t &socket, t_mode mode)
{
	Udata ret;
	Event tmp;

	if (mode.param.length() == 0)
	{
		tmp = Sender::mode_syntax_error(select_user(socket), mode.target, mode.option, "op", "nick");
		ret.insert(tmp);
		return ret;
	}
	else if (!is_user(mode.param))
	{
		tmp = Sender::mode_no_user_message(select_user(socket), mode.param);
		ret.insert(tmp);
		return ret;
	}

	Channel &target_channel = select_channel(mode.target);
	User &host_user = select_user(socket);
	if (target_channel.is_host(host_user))
	{
		if (target_channel.is_host(select_user(mode.param)))
		{
			target_channel.unset_host(select_user(mode.param));
		}
		ret = target_channel.send_all(host_user, host_user, "-o", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host_user, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_t_on(const uintptr_t &socket, t_mode mode)
{
	Udata ret;
	Event tmp;

	Channel &target_channel = select_channel(mode.target);
	User &host_user = select_user(socket);
	if (target_channel.is_host(host_user))
	{
		target_channel.set_flag(target_channel, mode);
		ret = target_channel.send_all(host_user, host_user, "+t", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host_user, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_t_off(const uintptr_t &socket, t_mode mode)
{
	Udata ret;
	Event tmp;

	Channel &target_channel = select_channel(mode.target);
	User &host_user = select_user(socket);
	if (target_channel.is_host(host_user))
	{
		target_channel.set_flag(target_channel, mode);
		ret = target_channel.send_all(host_user, host_user, "-t", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host_user, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_l_on(const uintptr_t &socket, t_mode mode)
{
	Udata ret;
	Event tmp;
	int limit_num = 0;
	std::stringstream get_limit_num_stream(mode.param);
	get_limit_num_stream >> limit_num;

	if (mode.param.length() == 0)
	{
		tmp = Sender::mode_syntax_error(select_user(socket), mode.target, mode.option, "limit", "limit");
		ret.insert(tmp);
		return ret;
	}
	else if (limit_num < 0)
	{
		tmp = Sender::mode_syntax_error_l_negative_num(select_user(socket), mode.target, mode.option, mode.param);
		ret.insert(tmp);
		return ret;
	}

	Channel &target_channel = select_channel(mode.target);
	User &host_user = select_user(socket);
	if (target_channel.is_host(host_user))
	{
		target_channel.set_flag(target_channel, mode);
		target_channel.set_member_limit(limit_num);
		get_limit_num_stream.str("");
		get_limit_num_stream.clear();
		get_limit_num_stream << "\b+l :" << limit_num;
		ret = target_channel.send_all(host_user, host_user, get_limit_num_stream.str(), MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host_user, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

Udata Database::command_mode_l_off(const uintptr_t &socket, t_mode mode)
{
	Udata ret;
	Event tmp;

	if (mode.param.length() != 0)
	{
		tmp = Sender::mode_wrong_message(socket, mode.param.at(mode.param.length() - 1));
		ret.insert(tmp);
		return ret;
	}

	Channel &target_channel = select_channel(mode.target);
	User &host_user = select_user(socket);
	if (target_channel.is_host(host_user))
	{
		target_channel.set_flag(target_channel, mode);
		ret = target_channel.send_all(host_user, host_user, "-l", MODE);
	}
	else
	{
		tmp = Sender::mode_error_not_op_message(host_user, mode.target);
		ret.insert(tmp);
	}
	return ret;
}

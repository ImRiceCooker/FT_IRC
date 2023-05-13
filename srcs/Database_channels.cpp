#include "Channel.hpp"
#include "Database.hpp"
#include "Udata.hpp"
#include "User.hpp"
#include "color.hpp"
#include <sys/_types/_size_t.h>

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
	tmp.set_host();
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

Udata Database::join_channel(User &joiner, const std::string &tmp_chan_name)
{
	Udata ret;
	Event tmp;
	std::string chan_name(tmp_chan_name);

	if (is_user_in_channel(joiner))
	{
		Event tmp2;
		tmp = Sender::join_message(joiner, joiner, chan_name);
		tmp2 = Sender::part_message(joiner, joiner, chan_name, "invaild : No Double join");
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
	else
	{
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
	{
		delete_channel(chan_name);
	}
	else
	{
		if (leaver == chan.get_host())
		{
			chan.set_host();
		}
	}
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
	{
		delete_channel(chan_name);
	}
	else
	{
		if (leaver == chan.get_host())
		{
			Udata tmp_;
			chan.set_host();
		}
	}
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
	if (channel.get_host() == host)
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

Udata Database::nick_channel(User &nicker, std::string &send_msg)
{
	Udata ret;
	Event tmp;
	User trash;

	Channel &channel = select_channel(nicker);
	ret = channel.send_all(nicker, trash, send_msg, NICK);
	channel.change_nick(nicker, send_msg);
	if (channel.get_host() == nicker)
	{
		channel.set_host(nicker);
	}
	return ret;
}

Udata Database::set_topic(User &sender, std::string &chan_name, std::string &topic)
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
	if (channel.get_host() == sender)
	{
		std::string topic_msg = "Topic was changed to " + topic;
		channel.set_topic(topic);
		ret = channel.send_all(sender, sender, topic_msg, TOPIC);
	}
	else
	{
		std::string topic_msg = "You do not have the authority to change the topic on this channel";
		ret = channel.send_all(sender, sender, topic_msg, TOPIC);
	}
	return ret;
}

Udata Database::command_mode_0(const uintptr_t &ident, std::string &chan_name)
{
	Udata ret;
	Event tmp;

	if (is_channel(chan_name) == false)
	{
		tmp = Sender::no_channel_message(select_user(ident), chan_name);
		ret.insert(tmp);
		return ret;
	}
	Channel &tmp_channel = select_channel(chan_name);
	User &host = select_user(ident);
	if (tmp_channel.get_host() == host)
	{
		// set_flag(host_user, channel, i인지 k인지, param)
		// tmp_channel.set_flag(host, tmp_channel, 0, param);
		// tmp = Sender::
		// ret.insert(tmp);
	}
	else
	{
		// tmp = Sender::mode_i_error_not_op_message(host, host.nickname_, target_name);
		ret.insert(tmp);
	}
	return ret;
}
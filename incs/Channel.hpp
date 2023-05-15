#pragma once

#include "Sender.hpp"
#include "Udata.hpp"
#include "type_mode.hpp"

#include <vector>
#include <algorithm>
#include <sstream>

enum e_send_switch
{
	JOIN,
	PART,
	PRIV,
	KICK,
	QUIT,
	NOTICE,
	TOPIC,
	WALL,
	NICK,
	MODE,
	WHO
};

enum e_channel_flag
{
	F_INVITE_ONLY = 0b001,
	F_KEY_NEEDED = 0b010,
	F_LIMITED_MEMBERSHIP = 0b100
};

class Channel
{
private:
	std::vector<User> connectors_;
	std::vector<uintptr_t> hosts_;
	std::vector<uintptr_t> invitations_;
	std::string name_;
	std::string topic_;
	std::string access_;
	int error_;

public:
	char channel_flag_;
	
	Channel();
	std::vector<uintptr_t> &get_hosts(void);
	void set_host(uintptr_t host_sock);
	void set_host(User &user);
	void unset_host(uintptr_t host_sock);
	void unset_host(User &host_user);
	bool is_host(uintptr_t client_sock);
	bool is_host(User &user);
	std::string &get_name(void);
	std::string &get_topic(void);
	std::string &get_access(void);
	void set_access(const std::string &access);
	void set_topic(std::string &topic);
	bool is_user(User &usr);
	void change_nick(User &usr, std::string new_nick);
	void add_user(User &joiner);
	void set_channel_name(std::string &chan_name);
	void delete_user(User &usr);
	std::string get_user_list_str(void);
	void set_flag(Channel &channel, t_mode &mode);

	Udata send_all(User &sender, User &target, std::string msg, int remocon);

	std::vector<User> &get_users(void);
	std::vector<User> sort_users(void);
	void seterror();
	int geterror();
	bool operator==(const Channel &t) const;
};

#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include "User/User.hpp"
#include "header.hpp"

class UserManager
{
private:
	map<int, User *> _users;

public:
	typedef map<int, User *>::const_iterator users_const_iter;
	typedef map<int, User *>::iterator users_iter;
	~UserManager();

	User *getUserWithFD(const int &fd) const;
	User *getUserWithName(const string &name) const;

	User *addUser(const string &name, const int &fd);

	void deleteChannelFromUser(Channel *const channel, User *const user);

	void deleteUser(const string &name);
	void deleteUser(const int &fd);
	void deleteUser(User *const user);
};

#endif

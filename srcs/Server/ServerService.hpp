#ifndef SERVER_SERVICE
#define SERVER_SERVICE

#include "header.hpp"
#include "Channel/ChannelManager.hpp"
#include "User/UserManager.hpp"
#include <map>

using namespace std;
class ServerService
{
private:
	ChannelManager _channelManager;
	UserManager _userManager;
	map<int, string> _passwordMap;
	string _password;

	ServerService operator=(const ServerService &ref);
	ServerService(const ServerService &ref);
	ServerService(void);

public:
	ServerService(const string &password);
	~ServerService(void);

	string getPassword(void) const;

	User *getUserWithName(const string &userName) const;

	User *getUserWithFD(const int &fd) const;

	Channel *getChannelWithName(const string &channelName) const;

	vector<User *> getUsersInChannel(const string &channelName) const;

	vector<Channel *> getChannelsFromUser(const string &userName) const;

	User *addUser(const string &userName, const int &fd);

	Channel *joinChannelWithUserName(const string &channelName, const string &userName);

	void deleteUserWithName(const string &userName);

	void deleteUserWithFD(const int &fd);

	void deleteUser(User *user);

	void partChannelWithUserName(const string &channelName, const string &userName);

	void changeUserName(const string &oldUserName, const string &newUserName);

	void insertPassword(const int &fd, const string &password)
	{
		_passwordMap[fd] = password;
	}

	const string &getMappedPassword(const int &fd)
	{
		map<int, string>::iterator it = _passwordMap.find(fd);
		return it->second;
	}

	class UserNotExist : public exception
	{
	public:
		const char *what() const throw();
	};

	class UserAlreadyExist : public exception
	{
	public:
		const char *what() const throw();
	};
	class ChannelNotExist : public exception
	{
	public:
		const char *what() const throw();
	};
	class ChannelAlreadyExist : public exception
	{
	public:
		const char *what() const throw();
	};
};

#endif
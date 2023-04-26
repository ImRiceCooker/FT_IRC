#ifndef SERVER_HPP
#define SERVER_HPP

#include "header.hpp"
#include "Command/Command.hpp"
#include "Server/ServerService.hpp"
#include <sys/socket.h>
#include <sys/poll.h>
#include <string>

class Command;

#define SERVER_PREFIX "irc.local"

class Server
{
	typedef void (Server::*pfunc)(int, string);

private:
	int _port;
	int _socket;
	struct pollfd _pollFDs[MAX_FD_SIZE];
	string _bufFDs[MAX_FD_SIZE];
	Command *_command;

	void _acceptConnections(void);
	void _sendResponse(void);

public:
	Server(const string &port, const string &password);
	~Server();
	const struct pollfd *getPollFDs() const;

	void prepare(void);

	void start(void);

	class InitServerException : public exception
	{
	public:
		const char *what(void) const throw();
	};
};

#endif

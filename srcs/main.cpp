#include "Server.hpp"
#include <signal.h>

/*
Server 클래스의 server_ptr_ 정적 포인터 변수에 생성된 객체를 저장하는 이유는 시그널 핸들러 함수에서 생성된 객체에 접근하기 위해서입니다.

시그널 핸들러 함수는 별도의 스레드에서 실행될 수 있으며, 이 스레드는 main() 함수의 스레드와 다른 스레드입니다.
이때, Server 클래스의 객체를 직접 참조할 경우 멀티스레딩 관련 문제가 발생할 수 있습니다.

따라서, Server 클래스의 객체를 정적 포인터 변수에 저장하고, 시그널 핸들러 함수에서 이 포인터를 통해 객체에 접근하는 것이 안전합니다.
이 방법을 사용하면 멀티스레딩 관련 문제를 방지할 수 있습니다.
*/

static void _sigint_handler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << BOLDCYAN << "Server Closed" << RESET << std::endl;
		Server::server_ptr_->server_sigint();
		exit(0);
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, _sigint_handler);
	system("clear");
	if (argc != 3)
	{
		std::cerr << RED << "err: Wrong Arguments" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string port(argv[1]);
	std::string password(argv[2]);
	Server server(port, password);
	Server::server_ptr_ = &server;
	server.start();
}

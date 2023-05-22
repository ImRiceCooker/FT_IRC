#pragma once

#include "User.hpp"
#include "Channel.hpp"

namespace debug
{
	static void showUsers(const std::vector<User> &target)
	{
		std::cout << BOLDMAGENTA << "[ User list ]  \n" << RESET << std::endl;
		int i = 0;
		std::vector<User>::const_iterator it;
		for (it = target.begin(); it != target.end(); ++it, ++i)
		{
			const User &user = *it;
			std::cout << "  " << BLUE << i + 1 << ". " << RESET
					  << BOLDWHITE << user.nickname_ << "(" << user.username_ << ", " << user.mode_ << ", " << user.unused_ << ", " << user.realname_ << ")" << RESET << std::endl;
		}
		std::cout << std::endl;
	}
}

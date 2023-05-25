#include <string>
#include <iostream>
#include "Color.hpp"

void exit_with_perror(const std::string& msg)
{
	std::cerr << BOLDRED << msg << RESET << std::endl;
	exit(EXIT_FAILURE);
}

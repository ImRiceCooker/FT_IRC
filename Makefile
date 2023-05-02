# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sesim <sesim@student.42seoul.kr>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/27 10:57:54 by sesim             #+#    #+#              #
#    Updated: 2023/02/09 14:56:30 by sesim            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

RM			=	rm -rf

OBJS_DIR	:=	objs/

SRCS		:=	$(wildcard *.cpp)
OBJS		:=	$(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))
NAME		:=	ircserv


all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	
$(OBJS_DIR)%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	$(RM) $(OBJS_DIR)
	$(RM) $(DEPS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean 
	@make all


.PHONY: all clean fclean re
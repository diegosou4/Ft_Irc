std::cout
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/26 21:49:04 by cbouvet           #+#    #+#              #
#    Updated: 2025/06/20 13:00:42 by cbouvet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CXX = @c++
CXX_FLAGS = -g -Werror -Wall -Wextra -std=c++98
SRC = 	main.cpp \
		Server.cpp    Revents.cpp    Commands.cpp    Utils.cpp \
		Client.cpp    Channel.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXX_FLAGS) -o $(NAME) $(OBJ)
	@echo "$(NAME) compiled!"

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)
	@echo ".o files removed!"

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) removed!"

re: fclean all

.PHONY: all clean fclean re


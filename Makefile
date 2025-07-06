# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/26 21:49:04 by cbouvet           #+#    #+#              #
#    Updated: 2025/07/06 10:29:58 by feden-pe         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CXX = @c++

CXX_FLAGS = -Werror -Wall -Wextra -std=c++98 -g

SRC = 	src/main.cpp \
		src/Server.cpp \
		src/Revents.cpp \
		src/AuthCmds.cpp \
		src/ChanCmds.cpp \
		src/MsgSenders.cpp \
		src/CmdUtils.cpp \
		src/Utils.cpp \
		src/Client.cpp \
		src/Channel.cpp

OBJ_PATH = obj/
SRC_PATH = src/

OBJ = $(patsubst $(SRC_PATH)%.cpp, $(OBJ_PATH)%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXX_FLAGS) -o $(NAME) $(OBJ)
	@echo "$(NAME) compiled!"

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp | $(OBJ_PATH)
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

clean:
	@rm -rf $(OBJ_PATH)
	@echo ".o files removed!"

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) removed!"

re: fclean all

# .PHONY: all clean fclean re


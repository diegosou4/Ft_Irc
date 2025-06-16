# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/16 22:11:14 by cbouvet           #+#    #+#              #
#    Updated: 2025/06/16 22:11:37 by cbouvet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = test_client
CXX = @c++
CXX_FLAGS = -Werror -Wall -Wextra -std=c++98
SRC = 	main.cpp TestClient.cpp
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

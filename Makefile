NAME = irc

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp Server.cpp Client.cpp channel.cpp Bot.cpp

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ) 
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)
clean :
	@rm -rf $(OBJ)
fclean : clean 
	@rm -rf $(NAME)
re : fclean all
NAME = a.out
SRC = main.cpp Server.cpp Client.cpp channel.cpp

all : $(NAME)

$(NAME) : $(SRC)
	c++ $(SRC) -o $(NAME)
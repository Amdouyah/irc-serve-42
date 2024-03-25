NAME = a.out
SRC = main.cpp Server.cpp Client.cpp 

all : $(NAME)

$(NAME) : $(SRC)
	c++ $(SRC) -o $(NAME)
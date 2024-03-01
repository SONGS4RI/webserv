NAME = webserv

SRC = main.cpp\
	ParseConfig.cpp\
	Block.cpp\
	Utils.cpp\
	Config.cpp\
	Response.cpp\
	SocketManager.cpp\
	Client.cpp\
	fortest.cpp\
	ResponseBody.cpp\
	statusCode/StatusCode.cpp\
	Server.cpp
	


OBJ = $(SRC:.cpp=.o)

CXX = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

clean :
	rm -rf $(OBJ)
	
fclean : clean
	rm -rf $(NAME)

re :
	make fclean
	make

.PHONY : all clean fclean re

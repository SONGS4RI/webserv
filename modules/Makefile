NAME = webserv

SRC = main.cpp\
	parseConfig/Config.cpp\
	parseConfig/ParseConfig.cpp\
	parseConfig/Block.cpp\
	request/Request.cpp\
	request/RequestBody.cpp\
	response/Response.cpp\
	response/ResponseBody.cpp\
	server/SocketManager.cpp\
	server/EventManager.cpp\
	server/Server.cpp\
	server/Client.cpp\
	utils/StatusCode.cpp\
	utils/HTTPInfo.cpp\
	utils/Utils.cpp\
	request_handler/RequestHandler.cpp


OBJ = $(SRC:.cpp=.o)

CXX = c++
CFLAGS = #-Wall -Werror -Wextra -std=c++98 -g -fsanitize=address

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

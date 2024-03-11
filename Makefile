NAME = webserv

SRC = modules/main.cpp\
	modules/parseConfig/Config.cpp\
	modules/parseConfig/ParseConfig.cpp\
	modules/parseConfig/Block.cpp\
	modules/request/Request.cpp\
	modules/request/RequestBody.cpp\
	modules/response/Response.cpp\
	modules/response/ResponseBody.cpp\
	modules/server/SocketManager.cpp\
	modules/server/EventManager.cpp\
	modules/server/Server.cpp\
	modules/server/Client.cpp\
	modules/utils/StatusCode.cpp\
	modules/utils/HTTPInfo.cpp\
	modules/utils/Utils.cpp\
	modules/request_handler/RequestHandler.cpp


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

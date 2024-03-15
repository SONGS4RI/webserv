NAME = webserv

SRC = modules/main.cpp\
	modules/config/Config.cpp\
	modules/config/ParseConfig.cpp\
	modules/config/Block.cpp\
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

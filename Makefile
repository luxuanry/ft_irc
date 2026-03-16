NAME = ircserv
CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

MAIN_SRCS = main.cpp Channel.cpp Loop.cpp Server.cpp User.cpp
CMD_SRCS = $(wildcard commands/*.cpp)
SRCS = $(MAIN_SRCS) $(CMD_SRCS)
OBJS = ${SRCS:%.cpp=%.o}

all: $(NAME)

%.o: %.cpp Channel.hpp Server.hpp User.hpp commands.hpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CPP) $(OBJS) $(CPPFLAGS) -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:
	fclean all

.PHONY: all clean fclean re
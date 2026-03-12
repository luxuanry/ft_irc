NAME = ircserv
CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

MAIN_SRCS = main.cpp
SRCS = $(MAIN_SRCS)
OBJS = ${SRCS:%.cpp=%.o}

all: $(NAME)

%.o: %.cpp
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
CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDE = -Iinclude -I../ft_lib/include
LIB = -L../ft_lib/lib -lft
SRC = src/checker.c src/get_next_line.c src/get_next_line_utils.c
NAME = ../checker

all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) $(LIB) -o $(NAME)

clean:
	rm -f $(NAME)

re: clean all
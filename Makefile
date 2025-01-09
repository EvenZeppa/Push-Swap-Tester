CC = gcc
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR = ft_lib
CHECKER_DIR = ft_checker
CHECKER = checker
LIBFT = $(LIBFT_DIR)/lib/libft.a
SRC = main.c
NAME = push_swap_tester

all: $(LIBFT) $(CHECKER) $(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) -I$(LIBFT_DIR)/include $(LIBFT) $(SRC) -o $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(CHECKER):
	make -C $(CHECKER_DIR)

clean:
	make -C $(LIBFT_DIR) clean
	make -C $(CHECKER_DIR) clean
	rm -f $(NAME)

fclean: clean
	make -C $(LIBFT_DIR) fclean
	make -C $(CHECKER_DIR) fclean
	rm -f $(NAME)

re: fclean all
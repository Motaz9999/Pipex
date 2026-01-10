NAME = pipex
CC = cc
CFLAGS = -g3 -Wall -Werror -Wextra

LIBFT_DIR =./libft
LIBFT =$(LIBFT_DIR)/libft.a

SRC = main.c \
	  find_path.c \
	  process.c
OBJ = $(SRC:.c=.o)

LINK = -L $(LIBFT_DIR) -lft

INC = -I. -I$(LIBFT_DIR)

all : $(NAME)
$(NAME) : $(LIBFT) $(OBJ)
	$(CC) $(CFLAGS) $(INC) $(OBJ) $(LINK) -o $(NAME)
$(LIBFT) :
	@make -C $(LIBFT_DIR)
clean:
	@rm -f $(OBJ)
	@make -C $(LIBFT_DIR) clean
fclean: clean
	@make -C $(LIBFT_DIR) fclean
	@rm -f $(NAME)
re: fclean all

.PHONY : all clean fclean re

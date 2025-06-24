MAKEFLAGS 			+= --no-print-directory
NAME				= philo
SRC_DIR				= src/
UTL_DIR				= utils/
ERR_DIR				= error/
CC					= @cc
#CFLAGS				= -Wall -Werror -Wextra
RM					= rm -rf

PHILO_DIR			=	$(SRC_DIR)philo.c \
							$(UTL_DIR)utils.c \
							$(UTL_DIR)philo_utils.c \
							$(ERR_DIR)error_handle.c \

SRCS 				= $(PHILO_DIR)
OBJ					= $(PHILO_DIR:.c=.o)

COLOR_YELLOW	=	\033[0;33m \033[1m
COLOR_GREEN		=	\033[0;32m \033[1m
COLOR_RED		=	\033[0;31m \033[1m
COLOR_END		=	\033[0m

all: 				$(NAME)

$(NAME): 			$(OBJ)
					@$(CC) $(OBJ) -o $(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_GREEN)[is ready!]\e[0m\n$(COLOR_END)"

clean:
					@$(RM) -r $(OBJ)
					@echo "\n\e[1m$(COLOR_YELLOW)objects	$(COLOR_RED)[are deleted!]\e[0m$(COLOR_END)"

fclean: 			clean
					@$(RM) $(NAME)
					@echo "\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_RED)[is deleted!]\e[0m\n$(COLOR_END)"

re: 				fclean all

.PHONY: 			all clean fclean re
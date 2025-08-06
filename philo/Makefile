MAKEFLAGS 			+= --no-print-directory # -fsanitize=address -g
NAME				= philo
OBJ_DIR				= obj/
SRC_DIR				= src/
UTL_DIR				= utils/
ERR_DIR				= error/
CC					= @cc
CFLAGS				= -Wall -Wextra -Werror -pthread -fsanitize=thread -g
RM					= rm -rf

PHILO_SRCS			=	$(SRC_DIR)philo.c \
							$(UTL_DIR)monitor.c \
							$(UTL_DIR)support.c \
							$(UTL_DIR)helpers.c \
							$(UTL_DIR)philo_utils.c \
							$(UTL_DIR)mutex.c \
							$(UTL_DIR)mandotary.c \
							$(ERR_DIR)error_handle.c \

SRCS 				= $(PHILO_SRCS)
OBJ					= $(addprefix $(OBJ_DIR), $(notdir $(SRCS:.c=.o)))

COLOR_YELLOW	=	\033[0;33m \033[1m
COLOR_GREEN		=	\033[0;32m \033[1m
COLOR_RED		=	\033[0;31m \033[1m
COLOR_END		=	\033[0m

all: 				$(NAME)

$(NAME): 			$(OBJ)
					@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_GREEN)[is ready!]\e[0m\n$(COLOR_END)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
					@mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: $(UTL_DIR)%.c
					@mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: $(ERR_DIR)%.c
					@mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

clean:
					@$(RM) $(OBJ_DIR)
					@echo "\n\e[1m$(COLOR_YELLOW)objects	$(COLOR_RED)[are deleted!]\e[0m$(COLOR_END)"

fclean: 			clean
					@$(RM) $(NAME)
					@echo "\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_RED)[is deleted!]\e[0m\n$(COLOR_END)"

re: 				fclean all

.PHONY: 			all clean fclean re debug production valgrind
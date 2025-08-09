NAME				= philo
OBJ_DIR				= obj/
SRC_DIR				= src/
CC					= @cc
CFLAGS				= -Wall -Wextra -Werror -pthread 
RM					= rm -rf

PHILO_SRCS			=	$(SRC_DIR)philo.c \
							$(SRC_DIR)monitor.c \
							$(SRC_DIR)support.c \
							$(SRC_DIR)helpers.c \
							$(SRC_DIR)philo_utils.c \
							$(SRC_DIR)mutex.c \
							$(SRC_DIR)mandotary.c \
							$(SRC_DIR)error_handle.c \

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

.PHONY: 			all clean fclean re
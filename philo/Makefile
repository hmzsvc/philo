MAKEFLAGS 			+= --no-print-directory
NAME				= philo
SRC_DIR				= src/
UTL_DIR				= utils/
ERR_DIR				= error/
CC					= @cc
CFLAGS				= -Wall -Wextra -Werror -pthread
# DEBUG_FLAGS		= -fsanitize=address -g  # Memory debugging
# THREAD_FLAGS		= -fsanitize=thread -g   # Thread debugging (sistem sorunu var)
RM					= rm -rf

PHILO_DIR			=	$(SRC_DIR)philo.c \
							$(UTL_DIR)monitor.c \
							$(UTL_DIR)utils.c \
							$(UTL_DIR)helpers.c \
							$(UTL_DIR)philo_utils.c \
							$(UTL_DIR)mutex_utils.c \
							$(UTL_DIR)philo_routine.c \
							$(ERR_DIR)error_handle.c \

SRCS 				= $(PHILO_DIR)
OBJ					= $(PHILO_DIR:.c=.o)

COLOR_YELLOW	=	\033[0;33m \033[1m
COLOR_GREEN		=	\033[0;32m \033[1m
COLOR_RED		=	\033[0;31m \033[1m
COLOR_END		=	\033[0m

all: 				$(NAME)

$(NAME): 			$(OBJ)
					@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_GREEN)[is ready!]\e[0m\n$(COLOR_END)"

debug:				CFLAGS += -fsanitize=address -g  # Memory leak detection & buffer overflow protection
debug:				$(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)Debug build with AddressSanitizer	$(COLOR_GREEN)[completed!]\e[0m\n$(COLOR_END)"

# ThreadSanitizer sistem sorunu nedeniyle devre dışı
# thread:			CFLAGS += -fsanitize=thread -g  
# thread:			$(NAME)

production:			CFLAGS += -O2  # Optimize for performance & reduce binary size
production:			$(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)Production build	$(COLOR_GREEN)[optimized!]\e[0m\n$(COLOR_END)"

clean:
					@$(RM) -r $(OBJ)
					@echo "\n\e[1m$(COLOR_YELLOW)objects	$(COLOR_RED)[are deleted!]\e[0m$(COLOR_END)"

fclean: 			clean
					@$(RM) $(NAME)
					@echo "\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_RED)[is deleted!]\e[0m\n$(COLOR_END)"

re: 				fclean all

valgrind:			$(NAME)  # Thread safety check with Helgrind tool
					@echo "\n\e[1m$(COLOR_YELLOW)Running Valgrind Helgrind (Thread Safety Check)...\e[0m$(COLOR_END)"
					@valgrind --tool=helgrind ./$(NAME) 5 800 200 200

.PHONY: 			all clean fclean re debug production valgrind
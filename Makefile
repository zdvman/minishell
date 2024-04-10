# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/10 10:31:12 by dzuiev            #+#    #+#              #
#    Updated: 2024/04/10 15:58:48 by dzuiev           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** # 
#                                 Minishell                                    #
#  Project structure:                                                          #
#   minishell/                                                                 #
#  ├── libft/              # Libft library files                               #
#  │   ├── Makefile                                                            #
#  │   └── ...             # Libft source files and headers                    #
#  ├── src/                # Source files for Minishell                        #
#  │   ├── core/           # Core functionality for both parts                 #
#  │   │   └── ...         # Core implementation files                         #
#  │   ├── mandatory/      # Mandatory specific files                          #
#  │   │   └── ...         # Mandatory implementation files                    #
#  │   ├── bonus/          # Bonus specific files (if any)                     #
#  │   │   └── ...         # Bonus implementation files                        #
#  │   └── main.c          # Main program entry point                          #
#  ├── includes/           # Header files                                      #
#  │   ├── minishell.h                                                         #
#  │   └── bonus_features.h                                                    #
#  ├── Makefile                                                                #
#  └── README.md                                                               #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
# Цвета для сообщений
# Colors for messages
GREEN=\033[0;32m
YELLOW=\033[0;33m
RED=\033[0;31m
BLUE=\033[0;34m
RESET=\033[0m
# **************************************************************************** #

# **************************************************************************** #
# Имя исполняемого файла для основной и бонусной частей проекта
# Name of the executable file for the main and bonus parts of the project
NAME = minishell
# **************************************************************************** #

# **************************************************************************** #
# Компилятор и флаги
# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
# **************************************************************************** #

# **************************************************************************** #
# Имя библиотеки libft и libminishell
# Name of the libft and libminishell libraries
LIBFT = $(LIBFT_DIR)/libft.a
LIBMINISHELL = libminishell.a
# **************************************************************************** #

# **************************************************************************** #
# Директории с исходным кодом и библиотеками
# Directories with source files and libraries
LIBFT_DIR = libft # path to the libft library
INCLUDES = includes # path to the header files
SRC_CORE_DIR = src/core # path to the core source files
OBJ_CORE_DIR = obj/core # path to the core object files
SRC_MANDATORY_DIR = src/mandatory # path to the mandatory source files
OBJ_MANDATORY_DIR = obj/mandatory # path to the mandatory object files
SRC_BONUS_DIR = src/bonus # path to the bonus source files
OBJ_BONUS_DIR = obj/bonus # path to the bonus object files
# **************************************************************************** #

# **************************************************************************** #
# Компиляция библиотеки libft
# Compilation of the libft library
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
# **************************************************************************** #

# **************************************************************************** #
# Исходные файлы для библиотеки libminishell
# Source files for the libminishell library
SRC_CORE = $(wildcard $(SRC_CORE_DIR)/*.c)
# Объектные файлы для библиотеки libminishell
# Object files for the libminishell library
OBJ_CORE = $(SRC_CORE:$(SRC_CORE_DIR)/%.c=$(OBJ_CORE_DIR)/%.o)
# Компиляция исходных core файлов в объектные файлы
# Compilation of core source files into object files
$(OBJ_CORE_DIR)/%.o: $(SRC_CORE_DIR)/%.c
	mkdir -p $(OBJ_CORE_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@
# Компиляция библиотеки libminishell
# Compilation of the libminishell library
$(LIBMINISHELL): $(OBJ_CORE)
	ar rcs $@ $^
# **************************************************************************** #

# **************************************************************************** #
# Исходные файлы для основной части проекта
# Source files for the main part of the project
SRC_MANDATORY = $(wildcard $(SRC_MANDATORY_DIR)/*.c)
# Объектные файлы для основной части проекта
# Object files for the main part of the project
OBJ_MANDATORY = $(SRC_MANDATORY:$(SRC_MANDATORY_DIR)/%.c=$(OBJ_MANDATORY_DIR)/%.o)
# Компиляция исходных файлов в объектные файлы
# Compilation of source files into object files
$(OBJ_MANDATORY_DIR)/%.o: $(SRC_MANDATORY_DIR)/%.c
	mkdir -p $(OBJ_MANDATORY_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@
# **************************************************************************** #

# **************************************************************************** #
# Исходные файлы для бонусной части проекта
# Source files for the bonus part of the project
SRC_BONUS = $(wildcard $(SRC_BONUS_DIR)/*.c)
# Объектные файлы для бонусной части проекта
# Object files for the bonus part of the project
OBJ_BONUS = $(SRC_BONUS:$(SRC_BONUS_DIR)/%.c=$(OBJ_BONUS_DIR)/%.o)
# Компиляция исходных файлов бонусной части проекта
# Compilation of source files for the bonus part of the project
$(OBJ_BONUS_DIR)/%.o: $(SRC_BONUS_DIR)/%.c
	mkdir -p $(OBJ_BONUS_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@
# **************************************************************************** #

# **************************************************************************** #
# Основная цель - сборка основной части проекта
# Main target - compilation of the main part of the project
all: $(NAME)
	@file $(YELLOW) $(NAME) $(RESET)
	@echo "$(GREEN)$(NAME) is ready!$(RESET)"
# **************************************************************************** #

# **************************************************************************** #
# Сборка основной части проекта
# Compilation of the main part of the project
$(NAME): $(LIBFT) $(LIBMINISHELL) $(OBJ_MANDATORY)
	$(CC) $(CFLAGS) -I$(INCLUDES) $(OBJ_MANDATORY) $(LIBFT) $(LIBMINISHELL) -o $(NAME)
# **************************************************************************** #

# **************************************************************************** #
# Цель и сборка бонусной части проекта
# Target and compilation of the bonus part of the proje
bonus: CFLAGS += -DBONUS # add a bonus flag to the compilation flags
bonus: $(LIBFT) $(LIBMINISHELL) $(OBJ_BONUS)
	$(CC) $(CFLAGS) -I$(INCLUDES) $(OBJ_BONUS) $(LIBFT) $(LIBMINISHELL) -o $(NAME)
	@file $(YELLOW) $(NAME) $(RESET)
	@echo "$(GREEN)Bonus part of $(NAME) is ready!$(RESET)"
# **************************************************************************** #

# **************************************************************************** #
# Очистка всех созданных файлов и папок с объектами
# Cleaning all created files and object folders
clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_CORE_DIR)
	rm -rf $(OBJ_MANDATORY_DIR)
	rm -rf $(OBJ_BONUS_DIR)
# **************************************************************************** #

# **************************************************************************** #
# Полная очистка проекта
# Full cleaning of the project
fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(LIBMINISHELL)
	rm -f $(NAME)
# **************************************************************************** #

# **************************************************************************** #
# Пересборка проекта
# Rebuilding the project
re: fclean all
# **************************************************************************** #

# **************************************************************************** #
# Указание фиктивных целей
# Specification of phony targets
.PHONY: all clean fclean re bonus
# **************************************************************************** #

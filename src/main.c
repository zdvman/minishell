/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 10:39:53 by dzuiev            #+#    #+#             */
/*   Updated: 2024/04/10 16:02:44 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/******************************************************************************/
/*                                Minishell                                   */
/* Project structure:                                                         */
/*  minishell/                                                                */
/* ├── libft/              # Libft library files                              */
/* │   ├── Makefile                                                           */
/* │   └── ...             # Libft source files and headers                   */
/* ├── src/                # Source files for Minishell                       */
/* │   ├── core/           # Core functionality for both parts                */
/* │   │   └── ...         # Core implementation files                        */
/* │   ├── mandatory/      # Mandatory specific files                         */
/* │   │   └── ...         # Mandatory implementation files                   */
/* │   ├── bonus/          # Bonus specific files (if any)                    */
/* │   │   └── ...         # Bonus implementation files                       */
/* │   └── main.c          # Main program entry point                         */
/* ├── includes/           # Header files                                     */
/* │   ├── minishell.h                                                        */
/* │   └── bonus_features.h                                                   */
/* ├── Makefile                                                               */
/* └── README.md                                                              */
/*                                                                            */
/******************************************************************************/

#include "../includes/minishell.h"

#ifdef BONUS
# include "../includes/minishell_bonus.h"
#endif

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	// Инициализация общих частей приложения
	common_init();

	// Расширенная инициализация, реализация которой будет отличаться
	// в зависимости от того, компилируется ли основная или бонусная часть
	extension_feature();

	// Основной цикл оболочки
	while (1) {
		// Взаимодействие с пользователем, выполнение команд и т.д.
	}

	// Очистка ресурсов
	cleanup();

	return (0);
}

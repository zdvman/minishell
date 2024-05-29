/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 10:39:53 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/29 17:31:16 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                 Minishell                                  */
/*  Project structure:                                                        */
/*   minishell/                                                               */
/*  ├── libft/                   Libft library files                          */
/*  │   ├── obj/                 Libft object files                           */
/*  │   ├── Makefile             Makefile to build the libft library          */
/*  │   └── ...                  Libft source files and headers               */
/*  ├── src/                     Source files for Minishell core functionality*/
/*  │   ├── mandatory/           Source files from mandatory sources          */
/*  │   ├── bonus/               Source files from bonus sources              */
/*  │   └── main.c               Main file for the Minishell project          */
/*  ├── includes/                Header files for all Minishell parts         */
/*  │   ├── minishell.h          Main headers for the project                 */
/*  │   └── minishell_bonus.h    Bonus headers for the project                */
/*  ├── obj/                     Object files for all Minishell parts         */
/*  │   ├── mandatory/           Object files from mandatory sources          */
/*  │   ├── bonus/               Object files from bonus sources              */
/*  ├── Makefile                 Makefile to build the minishell project      */
/*  └── README.md                Documentation of the project                 */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#ifdef BONUS
# include "../includes/minishell_bonus.h"
#endif

void	process_input(t_env **env, char *input)
{
	// t_token		*current;
	t_ast_node	*ast;

	g_signal = 0;
	add_history(input);
	get_tokens(input, env);
	expand_tokens(env);
	// current = (*env)->tokens;
	// print_tokens(current);
	ast = parse_tokens(env);
	if (ast)
	{
		generate_ast_diagram(ast);
		execute(ast, env);
	}
}

void	minishell_loop(t_env **env)
{
	char		*input;

	input = NULL;
	while (1)
	{
		g_global.g_signal = 0;
		input = read_multiline(env);
		if (!input)
			exit_minishell(env);
		else if (!*input && g_global.g_signal)
		{
			(*env)->exit_status = g_global.g_signal + 128;
			g_global.g_signal = 0;
			continue ;
		}
		if (*input)
			process_input(env, input);
		cleanup_loop(&input, env);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_env	*env;

	if (argc == 1)
	{
		shell_init(&env, envp);
		minishell_loop(&env);
		cleanup(&env, 0);
		return (0);
	}
	else
	{
		printf("Usage: ./%s\n", argv[0]);
		return (EXIT_FAILURE);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 13:42:33 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 15:35:50 by dzuiev           ###   ########.fr       */
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
/*  │   ├── ast_visualizer/      Source files for graphiz extension           */
/*  │   ├── mandatory/           Source files from mandatory sources          */
/*  │   ├── bonus/               Source files from bonus sources              */
/*  │   └── main.c               Main file for the Minishell project          */
/*  ├── includes/                Header files for all Minishell parts         */
/*  │   └── minishell.h          Main headers for the project                 */
/*  ├── obj/                     Object files for all Minishell parts         */
/*  │   ├── mandatory/           Object files from mandatory sources          */
/*  │   ├── bonus/               Object files from bonus sources              */
/*  ├── Makefile                 Makefile to build the minishell project      */
/*  └── README.md                Documentation of the project                 */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	star_check_in_token(char *token_value)
{
	char *value;
	char current_quote;

	value = token_value;
	current_quote = 0;
	if (contains(value, '*'))
	{
		while (*value)
		{
			current_quote = is_quote_open(value);
			if (*value == '\\'
				&& *(value + 1) == '*'
				&& (!current_quote || current_quote == '\"'))
			{
				value += 2;
				continue ;
			}
			if (*value == '*' && !current_quote)
				return (1);
			value++;
		}
	}
	return (0);
}

static int	bonus_loop_check(t_token **tmp, t_env **env)
{
	if ((*tmp)->type == TOKEN_BACKGROUND
		|| (*tmp)->type == TOKEN_OPEN_BRACKET
		|| (*tmp)->type == TOKEN_CLOSE_BRACKET
		|| (*tmp)->type == TOKEN_OR_IF
		|| (*tmp)->type == TOKEN_AND_IF
		|| (*tmp)->type == TOKEN_SEMI)
	{
		ft_putstr_fd((*tmp)->value, 2);
		ft_putstr_fd(" : is not supported\n", 2);
		cleanup_no_exit(env);
		return (1);
	}
	if (star_check_in_token((*tmp)->value))
	{
		ft_putstr_fd("* : is not supported\n", 2);
		cleanup_no_exit(env);
		return (1);
	}
	return (0);
}

static int	bonus_check(t_env **env)
{
	t_token			*tmp;

	tmp = NULL;
	tmp = (*env)->tokens;
	while (tmp)
	{
		if (bonus_loop_check(&tmp, env))
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

void	process_input(t_env **env, char *input)
{
	t_ast_node	*ast;

	add_history(input);
	get_tokens(input, env);
	if (bonus_check(env))
		return ;
	expand_tokens(env, (*env)->tokens, (*env)->tokens->prev);
	ast = parse_tokens(env);
	if (ast)
	{
		execute(ast, env);
	}
}

void	minishell_loop(t_env **env)
{
	char		*input;

	input = NULL;
	while (1)
	{
		g_signal = 0;
		input = read_multiline(env);
		(*env)->input = &input;
		if (!input)
			exit_minishell(env);
		else if (!*input && g_signal)
		{
			(*env)->exit_status = g_signal + 128;
			g_signal = 0;
			continue ;
		}
		if (*input)
			process_input(env, input);
		cleanup_loop(&input, env);
		g_signal = 0;
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 13:34:04 by samoore           #+#    #+#             */
/*   Updated: 2024/04/12 13:34:10 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_args(t_env *env)
{
	int	i;

	i = 0;
	while (env->tokens->type == WORD)
	{
		i++;
		env->tokens = env->tokens->right;
	}
	return (i);
}

void	build_command(t_env *env)
{
	t_token	*command;
	t_token	*tmp;
	int		i;

	command = env->tokens;
	add_path(env);
	if (!env->tokens->command_path)
		exit (1);	// need to add exit_error - bad filename
	i = 0;
	command->args = malloc(sizeof(char *) * (count_args(env) + 1));
	env->tokens = command;
	command->args[i++] = ft_strdup(env->tokens->string);
	env->tokens->type = COMMAND;
	env->tokens = env->tokens->right;
	while (env->tokens && env->tokens->type == WORD)
	{
		command->args[i++] = ft_strdup(env->tokens->string);
		free (env->tokens->string);
		tmp = env->tokens;
		env->tokens = env->tokens->right;
		free (tmp);
	}
	command->args[i] = 0;
	command->right = env->tokens;
}

void	check_brackets(t_env *env)
{
	int brackets;

	brackets = 0;
	while (env->tokens->type != END)
	{
		if (env->tokens->type == CLOSE_BRACKET)
			brackets--;
		if (brackets < 0)
		{
			printf("brackets\n");
			exit (1);        //need to decide how to handle this
		}
		if (env->tokens->type == OPEN_BRACKET)
			brackets++;
		env->tokens = env->tokens->right;
	}
	if (brackets)
	{
		printf ("brackets\n");
		exit (1);      //need to decide how to handle this
	}
}

int	parse_tokens(t_env *env)
{
	t_token	*current;
	t_token	*temp;

	current = env->tokens;
	temp = env->tokens;
	check_brackets(env);
	env->tokens = current;
	while (current && current->type != END)
	{
//		temp = env->tokens;
		env->tokens = current;

		if (current->type == WORD)
			build_command(env);
//		else if (current->type == REDIR_IN)
//			check_input(env);
//		else if (current->type == REDIR_OUT || current->type == REDIR_OUT_DBL)
//			check_output(env);	
//		env->tokens = temp;
		current = current->right;
	}
	env->tokens = env->token_head;
	return (0);
}
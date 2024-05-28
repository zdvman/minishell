/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 10:15:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/24 18:05:24 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	get_arg_count(t_token **current)
{
	t_token	*temp;
	int		arg_count;

	temp = *current;
	arg_count = 0;
	while (temp && temp->type == TOKEN_WORD)
	{
		arg_count++;
		temp = temp->next;
	}
	return (arg_count);
}

char	**copy_args(t_token **current, t_env **env)
{
	int		arg_count;
	char	**args;

	arg_count = get_arg_count(current);
	args = malloc(sizeof(char *) * (arg_count + 1));
	if (!args)
	{
		ft_perror("malloc failed");
		cleanup(env, EXIT_FAILURE);
	}
	args[arg_count] = NULL;
	arg_count = 0;
	while (*current && (*current)->type == TOKEN_WORD)
	{
		args[arg_count] = ft_strdup((*current)->value);
		if (!args[arg_count])
		{
			ft_perror("malloc failed");
			ft_free_args(&args);
			cleanup(env, EXIT_FAILURE);
		}
		*current = (*current)->next;
		arg_count++;
	}
	return (args);
}

char	**get_redir_arg(t_token **current, t_env **env)
{
	char	**arg;

	if (!(*current)->has_space
		&& ft_isnumber((*current)->value)
		&& is_redirection((*current)->next->type))
	{
		ft_putstr_fd("syntax error near unexpected token ", STDERR_FILENO);
		ft_putchar_fd('\'', STDERR_FILENO);
		ft_putstr_fd((*current)->value, STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		(*env)->syntax_error = 1;
		return (NULL);
	}
	arg = (char **)malloc(sizeof(char *) * 2);
	if (!arg)
	{
		ft_perror("malloc failed");
		cleanup(env, EXIT_FAILURE);
	}
	arg[0] = ft_strdup((*current)->value);
	arg[1] = NULL;
	if (!arg[0])
	{
		ft_perror("malloc failed");
		free(arg);
		cleanup(env, EXIT_FAILURE);
	}
	return (arg);
}

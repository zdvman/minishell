/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 12:34:01 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 13:06:08 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	syntax_redir_fd_error(t_token **current, t_env **env)
{
	if (!(*current)->has_space
		&& ft_isnumber((*current)->value)
		&& is_redirection((*current)->next->type))
	{
		ft_putstr_fd("syntax error near unexpected token ", STDERR_FILENO);
		ft_putchar_fd('`', STDERR_FILENO);
		ft_putstr_fd((*current)->value, STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		(*env)->syntax_error = 1;
		return (1);
	}
	return (0);
}

static char	**get_redir_arg(t_token **current, t_env **env)
{
	char	**arg;

	if (syntax_redir_fd_error(current, env))
		return (NULL);
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

t_ast_node	*parse_redirection(t_token **current, t_env **env)
{
	t_token_type	type;
	t_ast_node		*base_node;
	char			**arg;

	base_node = NULL;
	while (*current && is_redirection((*current)->type))
	{
		type = (*current)->type;
		*current = (*current)->next;
		if (!*current || (*current)->type != TOKEN_WORD)
		{
			ft_putstr_fd("syntax error near unexpected token `newline'\n", 2);
			(*env)->syntax_error = 1;
			(*env)->exit_status = 2;
			return (NULL);
		}
		arg = get_redir_arg(current, env);
		if (!arg)
			return (NULL);
		base_node = append_tree(base_node, create_ast_node(type, arg, env));
		*current = (*current)->next;
		if (*current && (*current)->type == TOKEN_WORD)
			base_node = append_tree(base_node, parse_command(current, env));
	}
	return (base_node);
}

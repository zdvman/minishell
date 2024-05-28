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

t_ast_node	*parse_brackets(t_token **current, t_env **env)
{
	t_ast_node	*node;
	t_ast_node	*redir_node;

	node = parse_expression(next_token(current), env);
	if (!node)
		return (NULL);
	if (!*current || (*current)->type != TOKEN_CLOSE_BRACKET)
	{
		ft_perror("Expected close bracket\n");
		ft_free_ast(&node);
		return (NULL);
	}
	*current = (*current)->next;
	if (*current && is_redirection((*current)->type))
	{
		redir_node = parse_redirection(current, env);
		node = tree_grafter(node, redir_node);
	}
	return (node);
}

t_ast_node	*parse_redirection(t_token **current, t_env **env)
{
	t_token_type	type;
	t_ast_node		*redir_node;
	t_ast_node		*base_node;
	char			**arg;

	base_node = NULL;
	while (*current && is_redirection((*current)->type))
	{
		type = (*current)->type;
		*current = (*current)->next;
		if (!*current || (*current)->type != TOKEN_WORD)
		{
			ft_perror("Expected filename for redirection\n");
			return (NULL);
		}
		arg = get_redir_arg(current, env);
		if (!arg)
			return (NULL);
		redir_node = create_ast_node(type, arg, env);
		if (!base_node)
			base_node = redir_node;
		else
		{
			redir_node->left = base_node;
			base_node = redir_node;
		}
		*current = (*current)->next;
		if (*current && (*current)->type == TOKEN_WORD)
		{
			redir_node = parse_command(current, env);
			base_node->left = redir_node;
		}
	}
	return (base_node);
}

t_ast_node	*parse_command(t_token **current, t_env **env)
{
	t_ast_node	*command_node;
	t_ast_node	*redir_node;
	char		**args;

	args = copy_args(current, env);
	redir_node = NULL;
	command_node = create_ast_node(TOKEN_WORD, args, env);
	if (!command_node)
	{
		ft_free_args(&args);
		return (NULL);
	}
	if (*current && is_redirection((*current)->type))
	{
		redir_node = parse_redirection(current, env);
		command_node = tree_grafter(command_node, redir_node);
	}
	return (command_node);
}

t_ast_node	*parse_term(t_token **current, t_env **env)
{
	t_ast_node	*node;

	if (!*current)
		return (NULL);
	if (*current && (*current)->type == TOKEN_OPEN_BRACKET)
		node = parse_brackets(current, env);
	else if (is_redirection((*current)->type))
		node = parse_redirection(current, env);
	else if ((*current)->type == TOKEN_WORD)
		node = parse_command(current, env);
	else
		return (NULL);
	if (!node)
		return (NULL);
	return (node);
}

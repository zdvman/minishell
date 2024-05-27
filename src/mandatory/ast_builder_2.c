/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 12:31:17 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/27 12:35:08 by dzuiev           ###   ########.fr       */
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

t_ast_node	*parse_expression(t_token **current, t_env **env)
{
	t_token_type	type;
	t_ast_node		*new_node;
	t_ast_node		*left;
	t_ast_node		*right;

	left = parse_term(current, env);
	if (!left)
		return (NULL);
	while (*current && (*current)->type != TOKEN_EOF
		&& is_control_op((*current)->type))
	{
		type = (*current)->type;
		(*current) = (*current)->next;
		right = parse_term(current, env);
		new_node = create_ast_node(type, NULL, env);
		new_node->left = left;
		new_node->right = right;
		left = new_node;
	}
	return (left);
}

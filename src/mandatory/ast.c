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

t_ast_node	*tree_grafter(t_ast_node *base_node, t_ast_node *new_node)
{
	t_ast_node	*last_node;

	if (!new_node)
		return (base_node);
	last_node = ft_last_node(new_node);
	last_node->left = base_node;
	base_node = new_node;
	return (base_node);
}

int	ast_output_is_valid(t_token **current, t_env **env)
{
	if ((*env)->syntax_error)
	{
		cleanup_no_exit(env);
		return (0);
	}
	if (*current && (*current)->type != TOKEN_EOF)
	{
		ft_putstr_fd("Unexpected token: ", STDERR_FILENO);
		print_token_name(*current);
		ft_putchar_fd('\n', STDERR_FILENO);
		cleanup_no_exit(env);
		return (0);
	}
	if ((*env)->ast == NULL)
	{
		ft_putstr_fd("Failed to parse tokens\n", STDERR_FILENO);
		cleanup_no_exit(env);
		return (0);
	}
	return (1);
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

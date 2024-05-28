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

t_ast_node	*new_ast_node(t_token_type type, char **args,
							t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*node;

	node = (t_ast_node *)malloc(sizeof(t_ast_node));
	if (!node)
	{
		ft_perror("Memory allocation failed for AST node\n");
		return (NULL);
	}
	node->type = type;
	node->args = args;
	node->left = left;
	node->right = right;
	return (node);
}

t_ast_node	*ft_last_node(t_ast_node *base_node)
{
	t_ast_node	*last_node;

	last_node = base_node;
	while (last_node->left)
		last_node = last_node->left;
	return (last_node);
}

t_ast_node	*create_ast_node(t_token_type type, char **args, t_env **env)
{
	t_ast_node	*node;

	node = (t_ast_node *)malloc(sizeof(t_ast_node));
	if (!node)
	{
		ft_perror("Memory allocation failed for AST node\n");
		cleanup(env, EXIT_FAILURE);
	}
	node->type = type;
	node->args = args;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

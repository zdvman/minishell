/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder_utils_1.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 12:42:43 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 13:15:28 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_token	**next_token(t_token **current)
{
	if (!current || !*current)
		return (NULL);
	*current = (*current)->next;
	return (current);
}

t_ast_node	*ft_last_node(t_ast_node *base_node)
{
	t_ast_node	*last_node;

	if (!base_node)
		return (NULL);
	last_node = base_node;
	while (last_node->left)
		last_node = last_node->left;
	return (last_node);
}

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

t_ast_node	*reroot_tree(t_ast_node *base_node, t_ast_node *new_node)
{
	if (!base_node)
		base_node = new_node;
	else
	{
		new_node->left = base_node;
		base_node = new_node;
	}
	return (base_node);
}

t_ast_node	*append_tree(t_ast_node *base_node,
				t_ast_node *new_node)
{
	t_ast_node	*last_node;

	last_node = NULL;
	if (!new_node)
		return (base_node);
	if (!base_node)
		return (new_node);
	last_node = ft_last_node(base_node);
	last_node->left = new_node;
	return (base_node);
}

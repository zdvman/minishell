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

t_token	**next_token(t_token **current)
{
	if (!current || !*current)
		return (NULL);
	*current = (*current)->next;
	return (current);
}

t_ast_node	*parse_tokens(t_env **env)
{
	t_token		*current;

	if (!env || !*env)
	{
		ft_perror("No environment to parse\n");
		cleanup_no_exit(env);
	}
	current = (*env)->tokens;
	if (!current || current->type == TOKEN_EOF)
		return (NULL);
	(*env)->ast = parse_expression(&current, env);
	if (ast_output_is_valid(&current, env))
		return ((*env)->ast);
	cleanup_no_exit(env);
	return (NULL);
}

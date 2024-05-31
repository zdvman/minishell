/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 12:29:53 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/27 12:30:05 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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

static int	ast_output_is_valid(t_token **current, t_env **env)
{
	if ((*env)->syntax_error)
	{
		(*env)->syntax_error = 0;
		cleanup_no_exit(env);
		return (0);
	}
	if (*current && (*current)->type != TOKEN_EOF)
	{
		ft_putstr_fd("Unexpected token: ", STDERR_FILENO);
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

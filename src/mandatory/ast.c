/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 10:15:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/21 17:38:04 by dzuiev           ###   ########.fr       */
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
	if (*current && (*current)->type != TOKEN_EOF)
	{
		printf("Unexpected token: ");
		print_token_name(*current);
		printf("\n");
		ft_perror("Syntax error\n");
		cleanup(env, 1);
		return (0);
	}
	if ((*env)->ast == NULL)
	{
		ft_perror("Failed to parse tokens\n");
		cleanup(env, 1);
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
		cleanup(env, EXIT_FAILURE);
	}
	current = (*env)->tokens;
	if (!current || current->type == TOKEN_EOF)
	{
		ft_perror("No input to parse\n");
		return (NULL);
	}
	(*env)->ast = parse_expression(&current, env);
	if (ast_output_is_valid(&current, env))
		return ((*env)->ast);
	cleanup(env, 1);
	return (NULL);
}

t_ast_node	*parse_brackets(t_token **current, t_env **env)
{
	t_ast_node	*node;
	t_ast_node	*redir_node;
	t_ast_node	*last_node;

	*current = (*current)->next;
	node = parse_expression(current, env);
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
		if (!redir_node)
		{
			ft_free_ast(&node);
			return (NULL);
		}
		last_node = ft_last_node(redir_node);
		last_node->left = node;
		node = redir_node;
	}
	return (node);
}

char	**get_redir_arg(t_token **current, t_env **env)
{
	char	**arg;

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
	t_ast_node		*redir_node;
	t_ast_node		*base_node;
	
	base_node = NULL;
	while (*current && is_redirection((*current)->type))
	{
		type = (*current)->type;
    	*current = (*current)->next;  // Move past the redirection token
		if (!*current || (*current)->type != TOKEN_WORD) {
			ft_perror("Expected filename for redirection\n");
			return (NULL);
		}
    	redir_node = create_ast_node(type, get_redir_arg(current, env), env);
		if (!base_node)
			base_node = redir_node;
		else
		{
			redir_node->left = base_node;
			base_node = redir_node;
		}
		*current = (*current)->next;  // Move past the filename
	}
    return (base_node);
}


t_ast_node *parse_command(t_token **current, t_env **env)
{
	t_ast_node	*command_node;
	t_ast_node 	*redir_node;
	t_ast_node	*last_node;
	char 		**args;

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
		if (!redir_node)
		{
			ft_free_ast(&command_node);
			return (NULL);
		}
		last_node = ft_last_node(redir_node);
		last_node->left = command_node;
		command_node = redir_node;
	}
	return (command_node);
}

t_ast_node *parse_term(t_token **current, t_env **env)
{
	t_ast_node	*node;

	if (!*current)
		return NULL;
	if (*current && (*current)->type == TOKEN_OPEN_BRACKET)
		node = parse_brackets(current, env);
	else if (is_redirection((*current)->type))
		node = parse_redirection(current, env);
	else if ((*current)->type == TOKEN_WORD)
		node = parse_command(current, env);
	else
		return (NULL);
	// else
	// {
	// 	ft_perror("Unexpected token: ");
	// 	print_token_name(*current);
	// 	printf("\n");
	// 	return (NULL);
	// }
	if (!node)
		return (NULL);
    return (node);  // Return the command wrapped with all applicable redirections
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
	while (*current && (*current)->type != TOKEN_EOF && is_control_op((*current)->type))
	{
		type = (*current)->type;
		(*current) = (*current)->next; // Move past the operator
		right = parse_term(current, env);
		// if (!right)
		// {
		// 	ft_free_ast(&left);
		// 	return NULL;
		// }
		new_node = create_ast_node(type, NULL, env);
		new_node->left = left;
		new_node->right = right;
		left = new_node;
	}
	return (left);
}

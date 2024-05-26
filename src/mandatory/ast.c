/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 10:15:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/26 23:25:03 by dzuiev           ###   ########.fr       */
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

t_ast_node	*append_tree_and_return_root(t_ast_node *base_node,
				t_ast_node *new_node)
{
	t_ast_node	*last_node;

	if (!new_node)
		return (base_node);
	last_node = ft_last_node(base_node);
	last_node->left = new_node;
	return (base_node);
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
	if ((*env)->syntax_error)
	{
		(*env)->syntax_error = 0;
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

int	found_fd_redirection_error(t_token **current, t_env **env)
{
	if (!(*current)->has_space
		&& ft_isnumber((*current)->value)
		&& is_redirection((*current)->next->type))
	{
		ft_putstr_fd("syntax error near unexpected token ", STDERR_FILENO);
		ft_putchar_fd('\'', STDERR_FILENO);
		ft_putstr_fd((*current)->value, STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		(*env)->syntax_error = 1;
		return (1);
	}
	return (0);
}

char	**get_redir_arg(t_token **current, t_env **env)
{
	char	**arg;

	if (found_fd_redirection_error(current, env))
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
			ft_perror("Expected filename for redirection\n");
			return (NULL);
		}
		arg = get_redir_arg(current, env);
		if (!arg)
			return (NULL);
		base_node = reroot_tree(base_node, create_ast_node(type, arg, env));
		*current = (*current)->next;
		if (*current && (*current)->type == TOKEN_WORD)
			base_node = append_tree_and_return_root(base_node,
					parse_command(current, env));
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

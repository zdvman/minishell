/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 10:15:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/15 21:13:55 by dzuiev           ###   ########.fr       */
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

t_ast_node	*create_ast_node(t_token_type type, char **args)
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

static void	check_ast_output(t_token **current, t_env **env)
{
	if (*current && (*current)->type != TOKEN_EOF)
	{
		printf("Unexpected token: ");
		print_token_name(*current);
		printf("\n");
		ft_perror("Syntax error\n");
		cleanup(env, EXIT_FAILURE);
	}
	if ((*env)->ast == NULL)
	{
		ft_perror("Failed to parse tokens\n");
		cleanup(env, EXIT_FAILURE);
	}
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
	check_ast_output(&current, env);
	return ((*env)->ast);
}

t_ast_node	*parse_brackets(t_token **current, t_env **env)
{
	t_ast_node	*node;

	if (!*current)
		return (NULL);
	*current = (*current)->next;
	node = parse_expression(current, env);
	if (!node)
		return (NULL);
	if (!*current || (*current)->type != TOKEN_CLOSE_BRACKET)
	{
		ft_perror("Expected close bracket\n");
		cleanup(env, EXIT_FAILURE);
		ft_free_ast(&node);
		return (NULL);
	}
	*current = (*current)->next;
	return (node);
}

t_ast_node *parse_redirection(t_token **current, t_env **env) {
    t_token_type type = (*current)->type;
    *current = (*current)->next;  // Move past the redirection token

    if (!*current || (*current)->type != TOKEN_WORD) {
        ft_perror("Expected filename for redirection\n");
        return NULL;
    }

    char **args = (char **)malloc(sizeof(char *) * 2);
    if (!args) {
        ft_perror("malloc failed");
		cleanup(env, EXIT_FAILURE);
        return NULL;
    }
    args[0] = ft_strdup((*current)->value);
    args[1] = NULL;
    *current = (*current)->next;  // Move past the filename

    t_ast_node *redir_node = create_ast_node(type, args);
    return redir_node;
}

t_ast_node *parse_command(t_token **current, t_env **env) {
    if (!*current || (*current)->type != TOKEN_WORD) {
        ft_perror("Expected command or argument\n");
        return NULL;
    }

    // This will collect all words until it encounters a token that isn't a word,
    // which might signify the end of the command or the start of a redirection or operator.
    char **args = copy_args(current, env);
    if (!args) {
        ft_perror("Failed to copy arguments for command\n");
        return NULL;  // Error handling in case args could not be copied
    }

    t_ast_node *command_node = create_ast_node(TOKEN_WORD, args);
    if (!command_node) {
        ft_free_args(&args); // Cleanup if node creation failed
        return NULL;
    }

    return command_node;
}




t_ast_node *parse_term(t_token **current, t_env **env) {
    if (!*current) return NULL;

    t_ast_node *base_node = NULL;  // This will eventually point to the command or the deepest nested expression
    t_ast_node *last_redirection = NULL;  // Keep track of the last parsed redirection

    // First, handle any leading redirections before a command or subexpression
    while (*current && is_redirection((*current)->type)) {
        t_ast_node *redir = parse_redirection(current, env);
        if (!redir) return NULL;  // Error during redirection parsing

        if (!base_node) {
            base_node = redir;
        } else {
            last_redirection->right = redir;  // Chain redirections in the order they appear
        }
        last_redirection = redir;  // Update the last redirection
    }

    // Depending on the current token, handle commands or grouped expressions
    if (*current && (*current)->type == TOKEN_OPEN_BRACKET) {
        base_node = parse_brackets(current, env);  // Parse brackets and its content recursively
        if (!base_node) return NULL;
    } else if (*current && (*current)->type == TOKEN_WORD) {
        base_node = parse_command(current, env);  // Parse the command
        if (!base_node) return NULL;
    } else {
        ft_perror("Unexpected token\n");
        return NULL; // Proper error handling
    }

    // If there were leading redirections, attach the parsed command/expression to the last redirection
    if (last_redirection) {
        last_redirection->right = base_node;
        base_node = last_redirection;  // The first redirection becomes the new base node
    }

    // Finally, handle any trailing redirections
    while (*current && is_redirection((*current)->type)) {
        t_ast_node *redir = parse_redirection(current, env);
        if (!redir) {
            ft_free_ast(&base_node);
            return NULL;  // Error during redirection parsing
        }

        // Chain trailing redirections to the right of the command or nested expression
        redir->left = base_node;
        base_node = redir;  // Update base_node to reflect the latest redirection wrapping
    }

    return base_node;  // Return the command wrapped with all applicable redirections
}




t_ast_node *parse_expression(t_token **current, t_env **env) {
    t_ast_node *left = parse_term(current, env);
    if (!left) return NULL; // Failed to parse left-hand side

    while (*current && (*current)->type != TOKEN_EOF && is_control_op((*current)->type)) {
        t_token_type operator_type = (*current)->type;
        (*current) = (*current)->next; // Move past the operator
        t_ast_node *right = parse_term(current, env);
        if (!right) {
            ft_free_ast(&left); // Cleanup left-hand side on failure
            return NULL;
        }

        t_ast_node *new_node = create_ast_node(operator_type, NULL);
        new_node->left = left;
        new_node->right = right;
        left = new_node; // Update 'left' to the newly formed compound node
    }
    return left; // Return the root of this subtree
}

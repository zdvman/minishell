/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 13:34:04 by samoore           #+#    #+#             */
/*   Updated: 2024/04/12 13:34:10 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_args(t_env *env)
{
	t_token *tmp;
	int		i;

	tmp = env->tokens;
	i = 0;
	while (tmp->type == WORD)
	{
		i++;
		tmp = tmp->right;
	}
	return (i);
}

t_list	*get_args_list(t_token *token, t_env *env)
{
	t_list	*new;
	t_token	*tmp;

	env->tokens->right = token;
	if (token->type != WORD)
		return (NULL);
	new = malloc(sizeof(t_list));
	new->entry = ft_strdup(token->string);
	tmp = token->right;
	free (token);
	new->next = get_args_list(tmp, env);
	return (new);
}

int	build_command(t_env *env)
{
	// if (add_path(env))
	// 	return (1);
	// if (!env->tokens->command_path)
	// {
	// 	printf("bad filename\n");
	// 	return (1);	// need to add exit_error - bad filename
	// }
	env->tokens->args_list = ft_calloc(sizeof(t_list), 1);
	env->tokens->args_list->entry = ft_strdup(env->tokens->string);
	env->tokens->args_head = env->tokens->args_list;
	env->tokens->type = COMMAND;
	env->tokens->args_list->next = get_args_list(env->tokens->right, env);
	return (0);
}

int	check_brackets(t_env *env)
{
	t_token	*current;
	int 	brackets;

	current = env->tokens;
	brackets = 0;
	while (current->type != END)
	{
		if (current->type == CLOSE_BRACKET)
			brackets--;
		if (brackets < 0)
		{
			printf("brackets\n");
			return (1);        //need to decide how to handle this
		}
		if (current->type == OPEN_BRACKET)
			brackets++;
		current = current->right;
	}
	if (brackets)
	{
		printf ("brackets\n");
		return (1);      //need to decide how to handle this
	}
	return (0);
}

int	get_redirect_string(t_env *env)
{
	t_token *current;

	if (env->tokens->right->type != WORD)
	{
		printf("bad redirect");
		return (1);
	}
	current = env->tokens;
	current = current->right;
	env->tokens->string = ft_strdup(current->string);
	free (current->string);
	env->tokens->right = current->right;
	free (current);
	return (0);
}

int	is_redirect_token(int type)
{
	if (type == REDIR_IN || type == REDIR_IN_DBL)
		return (1);
	if (type == REDIR_OUT || type == REDIR_OUT_DBL)
		return (1);
	return (0);
}

void	get_redirects_on_left(t_env *env)
{
	t_redirect *new;
	t_token		*command;
	t_token		*tmp;

	command = env->tokens;

	while (command->left && is_redirect_token(command->left->type))
	{
		env->tokens = env->tokens->left;
		new = malloc(sizeof(t_redirect));
		new->file = ft_strdup(env->tokens->string);
		new->type = env->tokens->type;
		new->next = NULL;
		if (command->redirects_head == NULL)
		{
			command->redirects_head = new;
			command->redirects = new;
		}
		else
		{
			new->next = command->redirects_head;
			command->redirects_head = new;
		}
		tmp = env->tokens;
		if (env->tokens->left)
		{
			command->left->left->right = command;
			command->left = command->left->left;
		}
		else
		{
			env->token_head = command;
			command->left = NULL;
		}
		free (tmp->string);
		free (tmp);
		env->tokens = command;
	}
	env->tokens = command;
}

void	get_redirects_on_right(t_env *env)
{
	t_redirect *new;
	t_token		*command;
	t_token		*tmp;

	command = env->tokens;
	while (is_redirect_token(env->tokens->right->type))
	{
		env->tokens = env->tokens->right;
		new = malloc(sizeof(t_redirect));
		new->file = ft_strdup(env->tokens->string);
		new->type = env->tokens->type;
		new->next = NULL;
		if (command->redirects_head == NULL)
		{
			command->redirects_head = new;
			command->redirects = new;
		}
		else
		{
			command->redirects->next = new;
			command->redirects = command->redirects->next;
		}
		tmp = env->tokens;			
		command->right = tmp->right;
		command->right->left = command;
		free (tmp->string);
		free (tmp);
		env->tokens = command;
	}
	env->tokens = command;
}

void	check_pipes(t_env *env)
{
	t_token *tmp;

	env->tokens = env->token_head;
	while (env->tokens->type != END)
	{
		if (env->tokens->type == COMMAND)
		{
			if (env->tokens->right->type == PIPE)
			{
				env->tokens->pipe = 1;
				tmp = env->tokens->right;
				env->tokens->right = tmp->right;
				tmp->right->left = env->tokens;
				free (tmp);
			}
		}
		env->tokens = env->tokens->right;
	}
	env->tokens = env->token_head;
}

int	parse_tokens(t_env *env)
{
	t_token	*current;

	current = env->tokens;
	if (check_brackets(env))
		return (1);
	while (env->tokens->type != END)
	{
		if (is_redirect_token(env->tokens->type))
		 	if ((get_redirect_string(env)) == 1)
				return (1);
		if (env->tokens->type == WORD)
			if ((build_command(env)) == 1)
				return (1);
		env->tokens = env->tokens->right;
	}
	link_tokens_left(env);
	env->tokens = env->token_head;
	while (env->tokens->type != END)
	{
		while (env->tokens->type != END && env->tokens->type != COMMAND)
			env->tokens = env->tokens->right;
		if (env->tokens->type == COMMAND)
		{
			get_redirects_on_left(env);
			get_redirects_on_right(env);
			env->tokens = env->tokens->right;
		}
	}
	return (0);
}
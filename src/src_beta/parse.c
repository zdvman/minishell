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

void	get_args(t_env *env)
{
	t_token	*throw_away;
	t_token	*tmp;
	int		i;

	i = 1;
	tmp = env->tokens;
	tmp = tmp->right;
	while (tmp && tmp->type == WORD)
	{
		env->tokens->args[i++] = ft_strdup(tmp->string);
		free (tmp->string);
		throw_away = tmp;
		tmp = tmp->right;
		free (throw_away);
	}
	env->tokens->args[i] = 0;
	env->tokens->right = tmp;
}

void	build_command(t_env *env)
{
	add_path(env);
	if (!env->tokens->command_path)
	{
		printf("bad filename\n");
		exit (1);	// need to add exit_error - bad filename
	}
	env->tokens->args = malloc(sizeof(char *) * (count_args(env) + 1));
	env->tokens->args[0] = ft_strdup(env->tokens->string);	
	env->tokens->type = COMMAND;
	env->tokens->in_fd = STDIN_FILENO;
	env->tokens->out_fd = STDOUT_FILENO;
	env->tokens->pipe = 0;
	env->tokens->out_fd = -1;
	env->tokens->in_fd = -1;
	get_args(env);
}

void	check_brackets(t_env *env)
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
			exit (1);        //need to decide how to handle this
		}
		if (current->type == OPEN_BRACKET)
			brackets++;
		current = current->right;
	}
	if (brackets)
	{
		printf ("brackets\n");
		exit (1);      //need to decide how to handle this
	}
}

void	get_redirect_string(t_env *env)
{
	t_token *current;

	if (env->tokens->right->type != WORD)
	{
		printf("bad redirect");
		exit (1);
	}
	current = env->tokens;
	current = current->right;
	env->tokens->string = ft_strdup(current->string);
	free (current->string);
	env->tokens->right = current->right;
	free (current);
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
			command->redirects->next = new;
			command->redirects = command->redirects->next;
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
	check_brackets(env);
	while (env->tokens->type != END)
	{
		if (is_redirect_token(env->tokens->type))
		 	get_redirect_string(env);
		if (env->tokens->type == WORD)
			build_command(env);
		env->tokens = env->tokens->right;
	}
	env->tokens = env->token_head;
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
	env->tokens = env->token_head;
	check_pipes(env);
	return (0);
}
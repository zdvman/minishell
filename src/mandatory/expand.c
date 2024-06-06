/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:25:09 by dzuiev            #+#    #+#             */
/*   Updated: 2024/04/24 13:25:09 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_backslach_out_of_quotes(char **input, char **current,
	t_dynamic_buffer *buf)
{
	(*input)++;
	*current = *input;
	buffer_append_char(buf, **input);
	(*input)++;
	*current = *input;
}

static void	expand_loop(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env)
{
	if (**input == '$')
		handle_dollar_sign(input, current, buf, env);
	else if (**input == '\'' || **input == '\"')
		handle_quotes(input, current, buf, env);
	else if (**input == '\\')
		handle_backslach_out_of_quotes(input, current, buf);
	else
	{
		buffer_append_char(buf, **input);
		(*input)++;
		*current = *input;
	}
}

char	*expand_word(t_env **env, char **input)
{
	t_dynamic_buffer	buf;
	char				*expanded;
	char				*current;
	char				*start;

	buffer_init(&buf);
	start = *input;
	current = *input;
	while (**input)
	{
		expand_loop(input, &current, &buf, env);
	}
	if (current != *input)
		buffer_append(&buf, current, *input - current);
	expanded = strdup(buf.data);
	buffer_free(&buf);
	*input = start;
	return (expanded);
}

t_token	*insert_vals(t_env **env, t_token **token)
{
	t_token	*prev;
	t_token	*next;
	t_token	*new;

	prev = (*token)->prev;
	next = (*token)->next;
	new = expand_wildcard((*token)->value, env, prev, next);
	if (!new)
		return (NULL);
	free ((*token)->value);
	free (*token);
	if (prev)
	{
		prev->next = new;
		return (new);
	}
	(*env)->head_token = new;
	(*env)->tokens = (*env)->head_token;
	return (new);
}

void	expand_tokens(t_env **env, t_token *token, t_token *prev)
{
	t_token	*tmp;
	char	*expanded;

	prev = (*env)->tokens->prev;
	token = (*env)->tokens;
	while (token)
	{
		tmp = NULL;
		if (token->type == TOKEN_WORD)
		{
			if (star_check_in_token(token->value)
				&& (!prev || !is_redirection(prev->type)))
				token->wildcard = 1;
			expanded = expand_word(env, &token->value);
			free(token->value);
			token->value = expanded;
			if (token->wildcard)
				tmp = insert_vals(env, &token);
		}
		if (tmp)
			token = tmp;
		prev = token;
		token = token->next;
	}
}

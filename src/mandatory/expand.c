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

int	insert_vals(t_env **env, t_token *token, t_token *prev)
{
	t_token	*next;

	next = token->next;
	if (expand_wildcard(token->value, env, prev, next))
	{
		free(token->value);
		free(token);
		token = next;
		return (1);
	}
	return (0);
}

void	expand_tokens(t_env **env)
{
	t_token	*token;
	t_token	*prev;
	char	*expanded;

	prev = NULL;
	token = (*env)->tokens;
	while (token)
	{
		if (token->type == TOKEN_WORD)
		{
			if (contains(token->value, '*'))
			{
				if (insert_vals(env, token, prev))
					continue ;
			}
			else
			{
				expanded = expand_word(env, &token->value);
				free(token->value);
				token->value = expanded;
			}
		}
		prev = token;
		token = token->next;
	}
}

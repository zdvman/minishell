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
		if (**input == '$')
			handle_dollar_sign(input, &current, &buf, env);
		else if (**input == '\'' || **input == '\"')
			handle_quotes(input, &current, &buf, env);
		else if (**input != '$' && **input != '\'' && **input != '\"')
			(*input)++;
	}
	if (current != *input)
		buffer_append(&buf, current, *input - current);
	expanded = strdup(buf.data);
	buffer_free(&buf);
	*input = start;
	return (expanded);
}

int	check_expand_wildcard(t_env **env, t_token **token, t_token **prev)
{
	t_token	*next;
	char	*expanded;

	if (contains((*token)->value, '*'))
	{
		next = (*token)->next;
		if (expand_wildcard((*token)->value, env, *prev, next))
		{
			free ((*token)->value);
			free ((*token));
			(*token) = next;
			return (1);
		}
	}
	else
	{
		expanded = expand_word(env, &(*token)->value);
		free((*token)->value);
		(*token)->value = expanded;
	}
	return (0);
}

void	expand_tokens(t_env **env)
{
	t_token	*token;
	t_token	*prev;

	prev = NULL;
	token = (*env)->tokens;
	while (token)
	{
		if (token->type == TOKEN_WORD)
			if (check_expand_wildcard(env, &token, &prev))
				continue ;
		prev = token;
		token = token->next;
	}
}

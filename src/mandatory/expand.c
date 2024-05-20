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

	// if (*input[0] == '$' && *input[1] != '$')
	// {
	// 	return (ft_strdup(get_env_variable(*env, *input)));
	// }
	buffer_init(&buf);
	start = *input;
	current = *input;
	while (**input)
	{
		if (**input == '$')
			handle_dollar_sign(input, &current, &buf, env);
		if (**input == '\'' || **input == '\"')
			handle_quotes(input, &current, &buf, env);
		if (**input != '$' && **input != '\'' && **input != '\"')
			(*input)++;
	}
	if (current != *input)
		buffer_append(&buf, current, *input - current);
	expanded = strdup(buf.data);
	buffer_free(&buf);
	*input = start;
	return (expanded);
}

void	expand_tokens(t_env **env)
{
	t_token	*token;
	t_token	*prev;
	t_token	*next;
	char	*expanded;

	prev = NULL;
	token = (*env)->tokens;
	while (token)
	{
		if (token->type == TOKEN_WORD)
		{
			if (contains(token->value, '*'))
			{
				next = token->next;
				if (expand_wildcard(token->value, env, prev, next))
				{
					free (token->value);
					free (token);
					token = next;
					continue ;
				}
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

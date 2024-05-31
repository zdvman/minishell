/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_quotes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 08:22:15 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/31 19:17:32 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_backslach(char **input, char **current,
				t_dynamic_buffer *buf)
{
	(*input)++;
	*current = *input;
	if (**input == '\"'
		|| **input == '\n'
		|| **input == '\\'
		|| **input == '$'
		|| **input == '`')
	{
		buffer_append_char(buf, **input);
		return (0);
	}
	else
	{
		buffer_append_char(buf, '\\');
		buffer_append_char(buf, **input);
	}
	(*input)++;
	return (1);
}

void	handle_double_quotes(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env)
{
	(*input)++;
	*current = *input;
	while (**input && **input != '\"')
	{
		if (**input == '$')
			handle_dollar_sign_in_quotes(input, current, buf, env);
		else if (**input == '\\')
			handle_backslach(input, current, buf);
		else if (**input == '`')
		{
			handle_backtick(input, env);
			return ;
		}
		else
		{
			buffer_append_char(buf, **input);
			(*input)++;
			*current = *input;
		}
	}
	*current = *input;
}

void	handle_single_quotes(char **input, char **current,
				t_dynamic_buffer *buf)
{
	(*input)++;
	*current = *input;
	while (**input && **input != '\'')
		(*input)++;
	if (**input == '\'')
	{
		if (*current != *input)
			buffer_append(buf, *current, *input - *current);
	}
	*current = *input;
}

void	handle_quotes(char **input, char **current, t_dynamic_buffer *buf,
			t_env **env)
{
	if (**input == '\"')
		handle_double_quotes(input, current, buf, env);
	else if (**input == '\'')
		handle_single_quotes(input, current, buf);
}

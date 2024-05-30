/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_input_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 14:44:11 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/30 14:21:28 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	new_current_quote(char current_quote, char c)
{
	if (current_quote == 0)
	{
		if (c == '\'' || c == '\"')
			return (c);
	}
	else if (current_quote == c)
		return (0);
	return (current_quote);
}

int	is_quote_open(const char *input)
{
	char	current_quote;
	int		i;

	i = 0;
	current_quote = 0;
	while (input[i] != '\0')
	{
		if (input[i] == '\\' && (input[i + 1] == current_quote))
			i++;
		else if ((input[i] == '\'' || input[i] == '\"')
			&& (i == 0 || input[i - 1] != '\\'))
			current_quote = new_current_quote(current_quote, input[i]);
		i++;
	}
	return (current_quote);
}

int	pipe_or_and_is_closed(char *buffer_str, int i)
{
	if (!buffer_str)
		return (1);
	while (buffer_str[i] != '\0')
	{
		if (buffer_str[i] == '|')
		{
			if (buffer_str[i + 1] == '\0')
				return (0);
			else if (buffer_str[i + 1] == '|')
			{
				if (buffer_str[i + 2] == '\0')
					return (0);
			}
		}
		else if (buffer_str[i] == '&')
		{
			if (buffer_str[i + 1] == '&')
			{
				if (buffer_str[i + 2] == '\0')
					return (0);
			}
		}
		i++;
	}
	return (1);
}

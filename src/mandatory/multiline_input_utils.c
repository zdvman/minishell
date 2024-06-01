/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_input_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 14:44:11 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/01 11:46:16 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	return_current_quote(const char *input, int *i, char *current_quote)
{
	*current_quote = input[*i];
	(*i)++;
	while (input[*i] && input[*i] != *current_quote)
	{
		if (*current_quote == '\"' && input[*i] == '\\')
			(*i)++;
		(*i)++;
	}
	if (input[*i] == *current_quote)
		*current_quote = 0;
	return (*current_quote);
}

int	is_quote_open(const char *input)
{
	char	current_quote;
	int		i;

	i = 0;
	current_quote = 0;
	while (input[i] != '\0')
	{
		if (input[i] == '\\' && (input[i + 1] == '\'' || input[i + 1] == '\"'))
		{
			i += 2;
			continue ;
		}
		if (input[i] && (input[i] == '\'' || input[i] == '\"'))
			current_quote = return_current_quote(input, &i, &current_quote);
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

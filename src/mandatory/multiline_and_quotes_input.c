/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_and_quotes_input.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 18:21:10 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/24 13:02:40 by dzuiev           ###   ########.fr       */
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
	return (current_quote != 0);
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

char	*read_multiline(t_env **env)
{
	t_dynamic_buffer	buf;
	char 				*line;

	buffer_init(&buf);
	while (1)
	{
		line = readline(buf.data[0] ? "> " : prompt(*env));
		if (g_signal)
        {
			(*env)->exit_status = g_signal + 128;
            g_signal = 0;
            ft_free_str(&line);
			buffer_clear(&buf);
            	break ;
        }
		if (line == NULL)
		{
			if (buf.data[0])
			{
				write(2, "unexpected EOF while looking for matching `\"\'\n", 46);
				buffer_free(&buf);
				return (ft_strdup(""));
			}
			else
			{
				buffer_free(&buf);
				return (NULL); // Exit the shell
			}
		}
		if (line[0] == '\0' && buf.data[0] == '\0')
		{
			buffer_free(&buf);
			return (line);
		}
		buffer_append(&buf, line, ft_strlen(line));
		if (is_quote_open(buf.data))
			buffer_append_char(&buf, '\n');
		ft_free_str(&line);
		if (!is_quote_open(buf.data))
		{
			if (pipe_or_and_is_closed(buf.data, 0))
				break ;
		}
	}
	line = ft_strdup(buf.data);
	buffer_free(&buf);
	return (line);
}

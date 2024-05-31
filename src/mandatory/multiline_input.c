/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_input.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 14:43:05 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/29 12:00:03 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*line_is_null_or_empty(char *line, t_dynamic_buffer buf)
{
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
			return (NULL);
		}
	}
	else
	{
		buffer_free(&buf);
		return (line);
	}
}

static char	*get_line_with_prompt(t_dynamic_buffer buf, t_env **env)
{
	char	*line;

	line = NULL;
	if (buf.data[0] == '\0')
		line = readline(prompt(*env));
	else
		line = readline("> ");
	return (line);
}

static int	g_signal_received(char **line, t_dynamic_buffer *buf, t_env **env)
{
	int	status;

	status = 0;
	if (g_signal)
	{
		(*env)->exit_status = g_signal + 128;
		g_signal = 0;
		ft_free_str(line);
		buffer_clear(buf);
		status = 1;
	}
	return (status);
}

char	*read_multiline(t_env **env)
{
	t_dynamic_buffer	buf;
	char				*line;

	buffer_init(&buf);
	while (1)
	{
		line = get_line_with_prompt(buf, env);
		if (g_signal_received(&line, &buf, env))
			break ;
		if (line == NULL || (line[0] == '\0' && buf.data[0] == '\0'))
			return (line_is_null_or_empty(line, buf));
		buffer_append(&buf, line, ft_strlen(line));
		if (is_quote_open(buf.data))
			buffer_append_char(&buf, '\n');
		ft_free_str(&line);
		if (!is_quote_open(buf.data))
			if (pipe_or_and_is_closed(buf.data, 0))
				break ;
	}
	line = ft_strdup(buf.data);
	buffer_free(&buf);
	return (line);
}

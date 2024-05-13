/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_and_quotes_input.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 18:21:10 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/10 14:16:04 by dzuiev           ###   ########.fr       */
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

char	*read_multiline(void)
{
	t_dynamic_buffer	buf;
	char				*line;

	line = readline("minishell> ");
	if (line == NULL || line[0] == '\0')
		return (line);
	buffer_init(&buf);
	buffer_append(&buf, line, ft_strlen(line));
	ft_free_str(&line);
	while (is_quote_open(buf.data))
	{
		line = readline("> ");
		if (!line)
		{
			write(2, "unexpected EOF while looking for matching `\"\'\n", 46);
			buffer_free(&buf);
			return (NULL);
		}
		buffer_append(&buf, line, ft_strlen(line));
		buffer_append_char(&buf, '\n');
		ft_free_str(&line);
	}
	if (buf.data[0])
		line = ft_strdup(buf.data);
	else
		line = ft_strdup("");
	buffer_free(&buf);
	return (line);
}

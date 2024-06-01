/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 16:25:37 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/31 10:41:19 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_dollar_special_case(char *input, t_env **env)
{
	write(2, "$", 1);
	ft_putchar_fd(*input, 2);
	ft_putstr_fd(": not supported in this version of minishell\n", 2);
	(*env)->syntax_error = 1;
	(*env)->exit_status = 2;
	return ;
}

void	handle_backtick(char **input, t_env **env)
{
	ft_putchar_fd(**input, 2);
	ft_putstr_fd(": not supported in this version of minishell\n", 2);
	(*env)->syntax_error = 1;
	(*env)->exit_status = 2;
	return ;
}

int	handle_dollar_backtick_exception(char **input, char **current, t_env **env)
{
	if (**input == '$' && is_dollar_special_case(*(*input + 1)))
	{
		handle_dollar_special_case(*input + 1, env);
		return (1);
	}
	else if (**input == '`' && !is_quote_open(*current))
	{
		handle_backtick(input, env);
		return (1);
	}
	return (0);
}

void	insert_nl_t_v(char **start, char **input, t_dynamic_buffer *buf)
{
	if (*start != *input)
		buffer_append(buf, *start, *input - *start);
	(*input)++;
	if (**input == 'n')
		buffer_append_char(buf, '\n');
	else if (**input == 'v')
		buffer_append_char(buf, '\v');
	else if (**input == 't')
		buffer_append_char(buf, '\t');
	(*input)++;
	*start = *input;
}

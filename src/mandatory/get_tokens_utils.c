/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 16:25:37 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/30 18:09:31 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_dollar_special_case(char *input, t_env **env)
{
	write(2, "$", 1);
	ft_putchar_fd(*input, 2);
	ft_putstr_fd(": not supported in this version of minishell\n", 2);
	(*env)->syntax_error = 1;
	return ;
}

void	handle_backtick(char **input, t_env **env)
{
	ft_putchar_fd(**input, 2);
	ft_putstr_fd(": not supported in this version of minishell\n", 2);
	(*env)->syntax_error = 1;
	return ;
}

int	handle_dollar_bakctick_exception(char **input, char **current, t_env **env)
{
	if (**input == '$' && is_dollar_special_case(*(*input + 1))
		&& !is_quote_open(*current))
	{
		handle_dollar_special_case(*input + 1, env);
		return (1);
	}
	else if (**input == '`' && !is_quote_open(*current))
	{
		handle_backtick(input, env);
		return (1);
	}
	if (**input == '$'
		&& *(*input + 1) == '\"'
		&& !is_quote_open(*current))
	{
		(*input)++;
		*current = *input;
		return (0);
	}
	return (0);
}

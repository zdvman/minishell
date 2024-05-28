/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 09:56:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/28 17:03:03 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	error_msg(char *cmd, int error_value)
{
	if (cmd)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		write(STDERR_FILENO, ": ", 2);
	}
	ft_putstr_fd(strerror(error_value), STDERR_FILENO);
	write(STDERR_FILENO, "\n", 1);
}

int	is_dollar_special_case(char c)
{
	return (c == '$'
		|| c == '!'
		|| c == '#'
		|| c == '*'
		|| c == '@'
		|| c == '('
		|| c == '{'
		|| c == '[');
}

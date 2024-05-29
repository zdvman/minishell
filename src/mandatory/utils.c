/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 09:56:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/29 11:03:46 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	put_3(char *s1, char *s2, char *s3)
{
	ft_putstr(s1);
	ft_putstr(s2);
	ft_putstr(s3);
}

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

void	add_child_pid(pid_t pid)
{
	if (g_global.pid_count < MAX_CHILDREN)
	{
		g_global.pid[g_global.pid_count++] = pid;
	}
}

int	contains(char *str, char target)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == target)
			return (1);
		i++;
	}
	return (0);
}

void	remove_child_pid(pid_t pid)
{
	int	i;

	i = 0;
	while (i < MAX_CHILDREN)
	{
		if (g_global.pid[i] == pid)
		{
			g_global.pid[i] = g_global.pid[--g_global.pid_count];
			break ;
		}
		i++;
	}
}

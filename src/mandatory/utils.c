/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 09:56:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/30 17:13:48 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	put_3(char *s1, char *s2, char *s3)
{
	ft_putstr_fd(s1, STDERR_FILENO);
	ft_putstr_fd(s2, STDERR_FILENO);
	ft_putstr_fd(s3, STDERR_FILENO);
}

void	error_msg(char *cmd, int error_value)
{
	ft_putstr("minishell: ");
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

static int	kill_list(int *list, int num)
{
	int	i;

	i = 0;
	while (i < num)
	{
		kill(list[i], SIGTERM);
		i++;
	}
	write(1, "\n", 1);
	return (0);
}

int	pid_list(t_cmd function, pid_t pid)
{
	static pid_t	list[MAX_CHILDREN];
	static int		num_children = 0;
	int				i;

	i = 0;
	if (function == ADD)
		list[num_children++] = pid;
	else if (function == REMOVE)
	{
		while (i < num_children)
		{
			if (list[i] == pid)
			{
				while (i++ <= num_children)
					list[i - 1] = list[i];
				num_children--;
			}
			i++;
		}
	}
	else if (function == KILL && num_children)
		num_children = kill_list(list, num_children);
	else if (function == GET_NUM)
		return (num_children);
	return (0);
}

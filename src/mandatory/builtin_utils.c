/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 18:42:01 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/11 14:28:59 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "echo"))
		return (1);
	if (!ft_strcmp(cmd, "cd"))
		return (1);
	if (!ft_strcmp(cmd, "pwd"))
		return (1);
	if (!ft_strcmp(cmd, "env"))
		return (1);
	if (!ft_strcmp(cmd, "unset"))
		return (1);
	if (!ft_strcmp(cmd, "exit"))
		return (1);
	if (!ft_strcmp(cmd, "export"))
		return (1);
	return (0);
}

int	contains(char *str, char target)
{
	int	i;

	i = ft_strlen(str);
	while (i >= 0)
	{
		if (str[i] == target)
			return (i + 1);
		i--;
	}
	return (0);
}

int	is_assignment(char *cmd)
{
	int	i;

	i = 0;
	if (cmd[0] != '_' && !ft_isalpha(cmd[0]))
		return (0);
	i++;
	while (cmd[i] && cmd[i] != '=')
	{
		if (cmd[0] != '_' && !ft_isalnum(cmd[0]))
			return (0);
		i++;
	}
	if (cmd[i] && cmd[i] == '=')
		return (1);
	return (0);
}

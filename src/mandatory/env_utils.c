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

char	*make_var(char *var, char *val)
{
	char	*tmp;
	char	*res;

	tmp = ft_strjoin(var, "=");
	res = ft_strjoin(tmp, val);
	free (tmp);
	return (res);
}

char	*get_var(char *env_var, char **vars)
{
	int		i;

	i = 0;
	if (!vars || !*vars)
		return (NULL);
	while (vars[i])
	{
		if (!ft_strncmp(vars[i], env_var, ft_strlen(env_var))
			&& vars[i][ft_strlen(env_var)] == '=')
			return (&vars[i][ft_strlen(env_var) + 1]);
		i++;
	}
	return (NULL);
}

char	*get_var_name(char *str)
{
	char	*res;
	int		i;

	i = 0;
	while (str[i] != '=')
		i++;
	res = ft_substr(str, 0, i);
	return (res);
}

int	valid_env_name(char *name)
{
	int	i;

	i = 0;
	if (name[i] != '_' && !ft_isalpha(name[i++]))
		return (0);
	while (name[i] && (name[i] == '_' || ft_isalnum(name[i])))
		i++;
	if (!name[i] || name[i] == '=')
		return (1);
	return (0);
}

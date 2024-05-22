/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:00:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/21 19:12:49 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*get_computer_name(t_env *env)
{
	char	*res;
	char	*tmp;
	int		i;
	int		j;
	int		k;

	tmp = get_env_var(env, "SESSION_MANAGER");
	i = 0;
	while (tmp[i] && tmp[i] != '/')
		i++;
	i++;
	j = i;
	while (tmp[j] && tmp[j] != '.')
		j++;
	res = ft_calloc(1, (j - i + 1));
	k = 0;
	while (i < j)
		res[k++] = tmp[i++];
	res[k] = 0;
	return (res);
}

char	*prompt(t_env *env)
{
	// char	*name;
	// char	*tmp;

	// if (!env->prompt)
	// {
	// 	name = get_computer_name(env);
	// 	tmp = ft_strjoin("@", name);
	// 	free (name);
	// 	name = NULL;
	// 	name = ft_strjoin(get_env_variable(env, "USER"), tmp);
	// 	free (tmp);
	// 	tmp = NULL;
	// 	tmp = ft_strjoin(name, ":");
	// 	env->prompt = tmp;
	// 	free (tmp);
	// }
	return (env->prompt);
}
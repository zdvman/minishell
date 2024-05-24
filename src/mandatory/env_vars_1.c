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

// char	*make_var(char *var, char *val)
// {
// 	char	*tmp;
// 	char	*res;

// 	tmp = ft_strjoin(var, "=");
// 	res = ft_strjoin(tmp, val);
// 	free (tmp);
// 	return (res);
// }

// char	*get_loc_var(t_env *env, char *env_var)
// {
// 	int		i;

// 	i = 0;
// 	if (!env->loc_vars)
// 		return (NULL);
// 	while (env->loc_vars[i])
// 	{
// 		if (!ft_strncmp(env->loc_vars[i], env_var, ft_strlen(env_var)) &&
// 			env->loc_vars[i][ft_strlen(env_var)] == '=')
// 				return (&env->loc_vars[i][ft_strlen(env_var) + 1]);
// 		i++;
// 	}
// 	return (NULL);
// }

// char	*get_env_var(t_env *env, char *env_var)
// {
// 	int		i;

// 	i = 0;
// 	if (!env->envp)
// 		return (NULL);
// 	while (env->envp[i])
// 	{
// 		if (!ft_strncmp(env->envp[i], env_var, ft_strlen(env_var)) &&
// 			env->envp[i][ft_strlen(env_var)] == '=')
// 				return (&env->envp[i][ft_strlen(env_var) + 1]);
// 		i++;
// 	}
// 	return (get_loc_var(env, env_var));
// }

// void	assign_variable(t_env *env, char *string)
// {
// 	char	*var;
// 	char	*val;
// 	int		i;

// 	i = 0;
// 	while (string[i] != '=')
// 		i++;
// 	var = ft_substr(string, 0, i);
// 	val = ft_substr(string, i + 1, ft_strlen(string) - i);
// 	if (get_env_var(env, var))
// 	{
// 		remove_env_var(env, var);
// 		add_env_var(env, var, val);
// 	}
// 		// replace_env_var(env, var, val);
// 	else if (get_loc_var(env, var))
// 	{
// 		remove_loc_var(env, var);
// 		add_loc_var(env, string);
// 	}
// 		// replace_loc_var(env, var, val);
// 	else
// 		add_loc_var(env, string);
// 	free (var);
// 	free (val);
// }

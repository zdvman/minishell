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

void	check_local(t_env **env, char *var, char *val)
{
	int	i;

	i = 0;
	while ((*env)->local_variables[i])
	{
		if (!ft_strncmp((*env)->local_variables[i], var, ft_strlen(var)))
		{
			free ((*env)->local_variables[i]);
			if (val && *val)
				(*env)->local_variables[i] = make_var(var, val);
			else
				remove_var(*env, i);
			return ;
		}
		i++;
	}
	insert_local(*env, make_var(var, val));
}

void	replace_or_add_env_var(t_env **env, char *var, char *val)
{
	int		i;

	i = 0;
	while ((*env)->envp[i])
	{
		if (!ft_strncmp((*env)->envp[i], var, ft_strlen(var)))
		{
			free ((*env)->envp[i]);
			if (val && *val)
				(*env)->envp[i] = make_var(var, val);
			else
				remove_var(*env, i);
			return ;
		}
		i++;
	}
	check_local(env, var, val);
}

void	assign_variable(t_env *env, char *string)
{
	char	*var;
	char	*val;
	int		i;

	i = 0;
	while (string[i] != '=')
		i++;
	var = ft_substr(string, 0, i);
	val = ft_substr(string, i + 1, ft_strlen(string) - i);
	replace_or_add_env_var(&env, var, val);
	free (var);
	free (val);
}

char	*get_env_variable(t_env *env, char *env_var)
{
	int		i;

	i = 0;
	while (env->envp[i])
	{
		if (!ft_strncmp(env->envp[i], env_var, ft_strlen(env_var)) &&
			env->envp[i][ft_strlen(env_var)] == '=')
				return (&env->envp[i][ft_strlen(env_var) + 1]);
		i++;
	}
	i = 0;
	while (env->local_variables[i])
	{
		if (!ft_strncmp(env->local_variables[i], env_var, ft_strlen(env_var)) &&
			env->local_variables[i][ft_strlen(env_var)] == '=')
				return (&env->local_variables[i][ft_strlen(env_var) + 1]);
		i++;
	}
	return (NULL);
}

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

void	insert_local(t_env *env, char *var)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->local_variables[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count + 2);
	while (i < count)
	{
		new_env[i] = env->local_variables[i];
		i++;
	}
	new_env[i++] = ft_strdup(var);
	free (env->local_variables);
	env->local_variables = new_env;
}

void	remove_var(t_env *env, int target)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->envp[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count);
	while (i < target)
	{
		new_env[i] = env->envp[i];
		i++;
	}
	while (i < count - 1)
	{
		new_env[i] = env->envp[i + 1];
		i++;
	}
	free (env->envp);
	env->envp = new_env;
}

void	add_env_var(t_env *env, char *var, char *val)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->envp[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count + 2);
	while (i < count)
	{
		new_env[i] = env->envp[i];
		i++;
	}
	new_env[i++] = make_var(var, val);
	free (env->envp);
	env->envp = new_env;
}

char	*check_envp(t_env *env, char *env_var)
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
	return (NULL);
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

void	export_var(t_env *env, char **args)
{
	if (!args[1])
		return ;
	if (!valid_env_name(args[1]))
	{
		ft_putstr("minishell: export: \'");
		ft_putstr(args[1]);
		ft_putstr("\': not a valid identifier\n");
		return ;
	}
	
	if (!is_assignment(args[1]) && !(check_envp(env, args[1])))
		if (get_env_variable(env, args[1]))
			add_env_var(env, args[1], get_env_variable(env, args[1]));
	if (is_assignment(args[1]))
		assign_variable(env, args[1]);
}

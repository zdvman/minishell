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

void	add_loc_var(t_env *env, char *var)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->loc_vars && env->loc_vars[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count + 2);
	while (i < count)
	{
		new_env[i] = env->loc_vars[i];
		i++;
	}
	new_env[i++] = ft_strdup(var);
	free (env->loc_vars);
	env->loc_vars = new_env;
}

void	remove_loc_var(t_env *env, char *name)
{
	char	**new_env;
	int		count;
	int		i;
	int		skip;

	count = 0;
	skip = 0;
	i = 0;
	while (env->loc_vars && env->loc_vars[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count);
	while (i < count)
	{
		if (strncmp(env->loc_vars[i], name, ft_strlen(name)) ||
		env->loc_vars[i][ft_strlen(name)] != '=')
			new_env[i - skip] = env->loc_vars[i];
		else
		{
			free (env->loc_vars[i]);
			// env->loc_vars[i] = NULL;
			skip = 1;
		}
		i++;
	}
	free (env->loc_vars);
	env->loc_vars = NULL;
	if (count > 1)
		env->loc_vars = new_env;
	else
		free(new_env);
}

void	remove_env_var(t_env *env, char *name)
{
	char	**new_env;
	int		count;
	int		i;
	int		skip;

	count = 0;
	skip = 0;
	i = 0;
	while (env->envp && env->envp[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count);
	while (i < count)
	{
		if (strncmp(env->envp[i], name, ft_strlen(name)) ||
		env->envp[i][ft_strlen(name)] != '=')
			new_env[i - skip] = env->envp[i];
		else
		{
			free (env->envp[i]);
			// env->envp[i] = NULL;
			skip = 1;
		}
		i++;
	}
	free (env->envp);
	env->envp = NULL;
	if (count > 1)
		env->envp = new_env;
	else
		free (new_env);
}

void	add_env_var(t_env *env, char *var, char *val)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->envp && env->envp[count])
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

void	assign_env_var(t_env *env, char *var)
{
	char	**new_env;
	int		count;
	int		i;
	char	*name;

	name = get_var_name(var);
	if (get_env_var(env, name))
		remove_env_var(env, name);
	if (get_loc_var(env, name))
		remove_loc_var(env, name);
	free (name);
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
	new_env[i] = ft_strdup(var);
	free (env->envp);
	env->envp = new_env;
}

// char	*check_envp(t_env *env, char *env_var)
// {
// 	int		i;

// 	i = 0;
// 	while (env->envp[i])
// 	{
// 		if (!ft_strncmp(env->envp[i], env_var, ft_strlen(env_var)) &&
// 			env->envp[i][ft_strlen(env_var)] == '=')
// 				return (&env->envp[i][ft_strlen(env_var) + 1]);
// 		i++;
// 	}
// 	return (NULL);
// }

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

int	export_var(t_env *env, char **args)
{
	if (!args[1])
		return (1);
	if (!valid_env_name(args[1]))
	{
		ft_putstr("minishell: export: \'");
		ft_putstr(args[1]);
		ft_putstr("\': not a valid identifier\n");
		return (2);
	}
	
	if (!is_assignment(args[1])) //&& !(check_envp(env, args[1])))
	{
		if (get_loc_var(env, args[1]))
		{
			// if (get_env_var(env, args[1]))
			// 	remove_loc_var(env, args[1]);
			add_env_var(env, args[1], get_loc_var(env, args[1]));
			remove_loc_var(env, args[1]);
		}
	}
	else
		assign_env_var(env, args[1]);
	return (0);
}

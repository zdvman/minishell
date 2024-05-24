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

void	remove_var(char *name, char ***vars)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	if (!(*vars))
		return ;
	while ((*vars)[count])
		count++;
	if (count == 0)
		return ;
	new_env = ft_calloc(sizeof(char *), count);
	while (i < count)
	{
		if (strncmp((*vars)[i], name, ft_strlen(name)) ||
		(*vars)[i][ft_strlen(name)] != '=')
			new_env[i] = (*vars)[i];
		else
		{
			free ((*vars)[i]);
			while ((*vars)[i + 1])
			{
				new_env[i] = (*vars)[i + 1];
				i++;
			}
			new_env[i] = NULL;
			free ((*vars));
			(*vars) = new_env;
			return ;
		}
		i++;
	}
	free (new_env);
}

void	assign_variable(t_env *env, char *string)
{
	char	*var;
	int		i;

	i = 0;
	while (string[i] != '=')
		i++;
	while (string[i] != '=')
		i++;
	var = ft_substr(string, 0, i);
	if (get_var(var, env->envp))
	{
		remove_var(var, &env->envp);
		add_var(string, &env->envp);
	}
	else if (get_var(var, env->loc_vars))
	{
		remove_var(var, &env->loc_vars);
		add_var(string, &env->loc_vars);
	}
	else
		add_var(string, &env->loc_vars);
	free (var);
}

void	add_var(char *var, char ***vars)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (((*vars))[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count + 2);
	while (i < count)
	{
		new_env[i] = (*vars)[i];
		i++;
	}
	new_env[i++] = ft_strdup(var);
	if (*vars)
		free (*vars);
	(*vars) = new_env;
}

int	export_var(t_env *env, char **args)
{
	int	i;

	i = 0;
	if (!args[1])
		return (1);
	if (!valid_env_name(args[1]))
	{
		ft_putstr("minishell: export: \'");
		ft_putstr(args[1]);
		ft_putstr("\': not a valid identifier\n");
		return (2);
	}	
	if (!is_assignment(args[1]))
	{
		if (get_var(args[1], env->loc_vars))
		{
			while (ft_strncmp(args[1], env->loc_vars[i], ft_strlen(args[1]))
				&& env->loc_vars[i][ft_strlen(args[1])] != '=')
			i++;
			add_var(env->loc_vars[i], &env->envp);
			remove_var(args[1], &env->loc_vars);
		}
	}
	else
		add_var(args[1], &env->envp);
	return (0);
}
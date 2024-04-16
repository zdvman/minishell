/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 14:03:17 by samoore           #+#    #+#             */
/*   Updated: 2024/04/12 14:03:33 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_path(t_env *env)
{
	int			i;

	i = 0;
	while (env->env[i] && ft_strncmp(env->env[i], "PATH=", 5))
		i++;
	return (&env->env[i][5]);
}

void	add_slash(char **paths)
{
	char	*tmp;
	int		i;

	i = 0;
	while (paths && paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		free (paths[i]);
		paths[i++] = tmp;
	}
}

void	check_path(t_env *env)
{
	char	*tmp;
	int		i;

	i = 0;
	while (env->paths && !env->tokens->command_path && env->paths[i])
	{
		tmp = ft_strjoin(env->paths[i++], env->tokens->string);
		if (access(tmp, F_OK) == 0)
		{
			env->tokens->command_path = ft_strdup(tmp);
			free (tmp);
			return ;
		}
		free (tmp);
	}
}

void	add_path(t_env *env)
{
	env->tokens->command_path = NULL;

	if (!strcmp(env->tokens->string, "exit"))
	{
		env->tokens->type = EXIT;
		return ;
	}
	if (access(env->tokens->string, F_OK) == -1)
		check_path(env);
	else
		env->tokens->command_path = ft_strdup(env->tokens->string);
	if (!env->tokens->command_path)
	{
		free_paths (env->paths);
		perror(env->tokens->string);
		exit (2);
	}
}

void	free_paths(char **paths)
{
	int	i;

	i = 0;
	while (paths && paths[i])
		free (paths[i++]);
	if (paths)
		free (paths);
}

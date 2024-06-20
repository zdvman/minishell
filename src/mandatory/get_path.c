/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:31:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/28 13:06:03 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	**get_path_dirs(char *path_value)
{
	char	**dirs;
	char	*current_dir;
	int		i;

	if (!path_value)
		return (NULL);
	dirs = ft_split(path_value, ':');
	if (!dirs)
		return (NULL);
	i = -1;
	while (dirs[++i])
	{
		current_dir = ft_strjoin(dirs[i], "/");
		ft_free_str(&dirs[i]);
		if (!current_dir)
		{
			ft_free_arr_of_str(&dirs);
			return (NULL);
		}
		dirs[i] = current_dir;
	}
	return (dirs);
}

char	**path_helper(t_env **env)
{
	char	**path_dirs;

	path_dirs = get_path_dirs(get_var("PATH", (*env)->envp));
	if (!path_dirs)
		path_dirs = get_path_dirs(get_var("PATH", (*env)->loc_vars));
	if (!path_dirs)
		return (NULL);
	return (path_dirs);
}

char	*get_path(char *cmd, t_env **env)
{
	char	**path_dirs;
	char	*path;
	int		i;

	path_dirs = path_helper(env);
	if (!path_dirs)
		return (NULL);
	if (!access(cmd, X_OK))
		return (ft_free_arr_of_str(&path_dirs), ft_strdup(cmd));
	i = -1;
	while (path_dirs[++i])
	{
		path = ft_strjoin(path_dirs[i], cmd);
		if (!path)
			break ;
		if (access(path, F_OK) == 0
			&& access(path, X_OK) == 0)
		{
			ft_free_arr_of_str(&path_dirs);
			return (path);
		}
		ft_free_str(&path);
	}
	ft_free_arr_of_str(&path_dirs);
	return (NULL);
}

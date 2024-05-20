/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:31:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/08 13:02:53 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_path_val(char *key, t_env **env)
{
	char	**current;
	int		key_len;

	if (!key
		|| !env
		|| !*env
		|| !(*env)->envp)
		// || !*((*env)->envp))
		return (NULL);
	key_len = ft_strlen(key);
	current = (*env)->envp;
	while (*current)
	{
		if (ft_strncmp(*current, key, key_len) == 0)
			return (*current + key_len);
		current++;
	}
	return (NULL);
}

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

char	*get_path(char *cmd, t_env **env)
{
	char	**path_dirs;
	char	*path;
	int		i;

	path_dirs = get_path_dirs(get_path_val("PATH=", env));
	if (!path_dirs)
		return (NULL);
	path = NULL;
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
	return (cmd);
}
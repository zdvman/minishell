/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 12:45:11 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/28 12:45:11 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_a_directory(char *cmd, t_env *env)
{
	struct stat	statbuf;
	char		**path;

	statbuf.st_mode = 0;
	stat(cmd, &statbuf);
	if (S_ISDIR(statbuf.st_mode))
	{
		path = path_helper(&env);
		if (cmd[ft_strlen(cmd) - 1] == '/' || cmd[0] == '/' || !path)
		{
			put_3("minishell: ", cmd, ": Is a directory\n");
			env->exit_status = 126;
		}
		else
		{
			put_3(cmd, ": command not found", "\n");
			env->exit_status = 127;
		}
		if (path)
			ft_free_arr_of_str (&path);
		return (1);
	}
	return (0);
}

int	is_exec(char *name)
{
	char	buf[4];
	int		fd;

	fd = open(name, O_RDONLY);
	if (fd == -1)
		return (0);
	if (read(fd, buf, 4) == 4)
	{
		if (ft_strncmp(buf, "\x7f""ELF", 4) == 0)
		{
			close(fd);
			return (1);
		}
	}
	close(fd);
	return (0);
}

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

void	pipe_fd_handler(int *fd, t_env **env, pid_t pid)
{
	if (pid == 0)
	{
		close(fd[0]);
		if_error(dup2(fd[1], STDOUT_FILENO) == -1, env);
		close(fd[1]);
	}
	else
	{
		close(fd[1]);
		if_error(dup2(fd[0], STDIN_FILENO) == -1, env);
		close(fd[0]);
	}
}

int	cmd_is_not_valid_no_path(char *cmd, t_env **env)
{
	if (!access(cmd, F_OK) && access(cmd, X_OK) == -1)
	{
		put_3("minishell: ", cmd, ": Permission denied\n");
		(*env)->exit_status = 126;
		return (1);
	}
	if (((access(cmd, F_OK)) == -1)
		|| ((access(cmd, F_OK)) == 0 && access(cmd, X_OK)))
	{
		put_3(cmd, ": command not found", "\n");
		(*env)->exit_status = 126;
		return (1);
	}
	if (access(cmd, F_OK))
	{
		put_3("minshell: ", cmd, ": No such file or directory\n");
		(*env)->exit_status = 127;
		return (1);
	}
	return (0);
}

int	cmd_is_not_valid(char *cmd, t_env **env)
{
	char	*path;

	if (!cmd || is_a_directory(cmd, *env))
		return (1);
	path = get_path(cmd, env);
	if (!path)
		return (cmd_is_not_valid_no_path(cmd, env));
	else if ((!access(path, F_OK) && is_exec(path) && access(path, X_OK) == -1))
		return (put_3("minishell: ", cmd, ": Permission denied\n"),
			(*env)->exit_status = 126, free (path), 1);
	else if (access(cmd, F_OK) && !path)
		return (put_3("minshell: ", cmd, ": No such file or directory\n"),
			(*env)->exit_status = 127, free (path), 1);
	else if (((access(path, F_OK)) == -1 || ((access(path, F_OK)) == 0
				&& path && !is_exec(path))))
		return (put_3(cmd, ": command not found", "\n"),
			(*env)->exit_status = 127, free (path), 1);
	else
		free (path);
	return (0);
}

void	handle_fd(t_env **env)
{
	if ((*env)->fd_out != -1)
	{
		if (dup2((*env)->fd_out, STDOUT_FILENO) == -1)
		{
			error_msg(NULL, errno);
			cleanup(env, EXIT_FAILURE);
		}
		close((*env)->fd_out);
		(*env)->fd_out = -1;
	}
	if ((*env)->fd_in != -1)
	{
		if (dup2((*env)->fd_in, STDIN_FILENO) == -1)
		{
			error_msg(NULL, errno);
			cleanup(env, EXIT_FAILURE);
		}
		close((*env)->fd_in);
		(*env)->fd_in = -1;
	}
}

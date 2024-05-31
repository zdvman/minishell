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

int	cmd_is_not_valid(char *cmd, t_env **env)
{
	char	*path;

	if (!cmd)
		return (1);
	if (is_builtin(cmd))
		return (0);
	path = NULL;
	path = get_path(cmd, env);
	if (!path
		|| access(path, F_OK) == -1
		|| access(path, X_OK) == -1)
	{
		if (contains(cmd, '/'))
			put_3("bash: ", cmd, ": No such file or directory");
		else
		{
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": command not found", STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		}
		(*env)->exit_status = 127;
		return (127);
	}
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 12:44:43 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/28 12:44:43 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	wait_for_process(pid_t pid, t_env **env)
{
	int	status;

	status = 0;
	(*env)->exit_status = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		(*env)->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		(*env)->exit_status = WTERMSIG(status);
	else if (WIFSTOPPED(status))
		(*env)->exit_status = WSTOPSIG(status);
}

void	if_error(bool status, t_env **env)
{
	if (status)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
}

void	set_origin_fd(int *origin_fd)
{
	origin_fd[0] = dup(STDIN_FILENO);
	origin_fd[1] = dup(STDOUT_FILENO);
}

void	restore_origin_fd(int *origin_fd, t_env **env)
{
	if (origin_fd[0] != -1)
	{
		if (dup2(origin_fd[0], STDIN_FILENO) == -1)
		{
			error_msg(NULL, errno);
			cleanup(env, EXIT_FAILURE);
		}
		close(origin_fd[0]);
		origin_fd[0] = -1;
	}
	if (origin_fd[1] != -1)
	{
		if (dup2(origin_fd[1], STDOUT_FILENO) == -1)
		{
			error_msg(NULL, errno);
			cleanup(env, EXIT_FAILURE);
		}
		close(origin_fd[1]);
		origin_fd[1] = -1;
	}
}

int	here_doc_signal_handler(t_env **env, int fd, int *origin_fd)
{
	int	status;

	status = 0;
	close(fd);
	if (g_signal)
	{
		(*env)->exit_status = g_signal + 128;
		g_signal = 0;
		status = 1;
	}
	restore_origin_fd(origin_fd, env);
	return (status);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:00:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/24 17:39:59 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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

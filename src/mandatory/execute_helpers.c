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

void	error_msg(char *cmd, int error_value)
{
	if (cmd)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		write(STDERR_FILENO, ": ", 2);
	}
	ft_putstr_fd(strerror(error_value), STDERR_FILENO);
	write(STDERR_FILENO, "\n", 1);
}

void	wait_for_process(pid_t pid, t_env **env)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
	if ((*env)->exit_status != -1)
		return ;
	if (WIFEXITED(status))
		(*env)->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		(*env)->exit_status = WTERMSIG(status);
	else if (WIFSTOPPED(status))
		(*env)->exit_status = WSTOPSIG(status);
	else if (WIFCONTINUED(status))
	{
		ft_putstr_fd("Process continued\n", STDERR_FILENO);
		(*env)->exit_status = 0;
	}
}

void	if_error(bool status, t_env **env)
{
	if (status)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
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
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": command not found", STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	return (0);
}

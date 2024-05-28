/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes_commands.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 12:49:48 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/28 12:49:48 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	execute_child(t_ast_node *node, t_env **env)
{
	if (is_assignment(node->args[0]))
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		assign_variable(*env, node->args[0]);
		execve(get_path(node->args[1], env), &node->args[1], (*env)->envp);
	}
	else
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execve(get_path(node->args[0], env), node->args, (*env)->envp);
	}
	error_msg(node->args[0], errno);
	cleanup(env, EXIT_FAILURE);
}

pid_t	execute_command(t_ast_node *node, t_env **env)
{
	pid_t	pid;
	int		origin_fd[2];

	set_origin_fd(origin_fd);
	handle_fd(env);
	if (is_builtin(node->args[0]))
	{
		execute_builtin(env, node->args);
		return (0);
	}
	else if (is_assignment(node->args[0]) && !node->args[1])
	{
		assign_variable(*env, node->args[0]);
		return (0);
	}
	pid = fork();
	if_error(pid == -1, env);
	if (pid == 0)
		execute_child(node, env);
	else
	{
		restore_origin_fd(origin_fd, env);
		wait_for_process(pid, env);
	}
	return (pid);
}

void	execute_pipe(t_ast_node *node, t_env **env)
{
	int		fd[2];
	int		origin_fd[2];
	pid_t	pid;

	set_origin_fd(origin_fd);
	handle_fd(env);
	if_error(pipe(fd) == -1, env);
	pid = fork();
	if_error(pid == -1, env);
	if (pid == 0)
	{
		pipe_fd_handler(fd, env, pid);
		execute(node->left, env);
		exit((*env)->exit_status);
	}
	else
	{
		pipe_fd_handler(fd, env, pid);
		execute(node->right, env);
		restore_origin_fd(origin_fd, env);
		wait_for_process(pid, env);
	}
}
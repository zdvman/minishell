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
		assign_variable(*env, &(*env)->loc_vars, node->args[0]);
		execve(get_path(node->args[1], env), &node->args[1], (*env)->envp);
		(*env)->exit_status = 127;
	}
	else
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (get_path(node->args[0], env))
			execve(get_path(node->args[0], env), node->args, (*env)->envp);
		else
			execve(node->args[0], node->args, (*env)->envp);
	}
	error_msg(node->args[0], errno);
	cleanup(env, EXIT_FAILURE);
}

void	execute_command(t_ast_node *node, t_env **env)
{
	pid_t	pid;
	int		origin_fd[2];

	set_origin_fd(origin_fd);
	if (is_builtin(node->args[0]))
		return (execute_builtin(env, node->args));
	else if (is_assignment(node->args[0]) && !node->args[1])
	{
		assign_variable(*env, &(*env)->loc_vars, node->args[0]);
		return ;
	}
	if (cmd_is_not_valid(node->args[0], env))
		return ;
	pid = fork();
	if_error(pid == -1, env);
	if (pid == 0)
		execute_child(node, env);
	else
	{
		pid_list(ADD, pid);
		restore_origin_fd(origin_fd, env);
		wait_for_process(pid, env);
		pid_list(REMOVE, pid);
	}
}

void	execute_pipe(t_ast_node *node, t_env **env)
{
	int		fd[2];
	int		origin_fd[2];
	pid_t	pid;

	set_origin_fd(origin_fd);
	if (pipe(fd) == -1)
		return (error_msg("pipe", errno), restore_origin_fd(origin_fd, env));
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
		pid_list(ADD, pid);
		pipe_fd_handler(fd, env, pid);
		execute(node->right, env);
		restore_origin_fd(origin_fd, env);
		wait_for_process(pid, env);
		pid_list(REMOVE, pid);
	}
}

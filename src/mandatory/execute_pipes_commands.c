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

void	add_child_pid(pid_t pid, t_env **env)
{
	if ((*env)->pid_count < MAX_CHILDREN)
	{
		(*env)->pid[(*env)->pid_count++] = pid;
	}
}

void	remove_child_pid(pid_t pid, t_env **env)
{
	int	i;

	i = 0;
	while (i < MAX_CHILDREN)
	{
		if ((*env)->pid[i] == pid)
		{
			(*env)->pid[i] = (*env)->pid[--(*env)->pid_count];
			break ;
		}
		i++;
	}
}

void	execute_command(t_ast_node *node, t_env **env)
{
	pid_t	pid;
	int		origin_fd[2];

	set_origin_fd(origin_fd);
	handle_fd(env);
	if (is_builtin(node->args[0]))
		return (execute_builtin(env, node->args));
	else if (is_assignment(node->args[0]) && !node->args[1])
		return (assign_variable(*env, node->args[0]));
	pid = fork();
	if_error(pid == -1, env);
	if (pid == 0)
		execute_child(node, env);
	else
	{
		add_child_pid(pid, env);
		restore_origin_fd(origin_fd, env);
		wait_for_process(pid, env);
		remove_child_pid(pid, env);
	}
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

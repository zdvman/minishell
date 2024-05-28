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

void	execute_semi(t_ast_node *node, t_env **env)
{
	execute(node->left, env);
	execute(node->right, env);
}

void	execute_and(t_ast_node *node, t_env **env)
{
	execute(node->left, env);
	if ((*env)->exit_status == 0)
		execute(node->right, env);
}

void	execute_or(t_ast_node *node, t_env **env)
{
	execute(node->left, env);
	if ((*env)->exit_status != 0)
		execute(node->right, env);
}

void	execute_background(t_ast_node *node, t_env **env)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
	if (pid == 0)
	{
		execute(node->left, env);
		exit((*env)->exit_status);
	}
	else
	{
		printf("Process running in background with PID %d\n", pid);
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

pid_t	execute_command(t_ast_node *node, t_env **env)
{
	pid_t	pid;

	handle_fd(env);
	(*env)->exit_status = -1;
	if (is_builtin(node->args[0]))
	{
		(*env)->exit_status = execute_builtin(env, node->args);
		return (0);
	}
	else if (is_assignment(node->args[0]) && !node->args[1])
	{
		assign_variable(*env, node->args[0]);
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
	if (pid == 0)
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
	else
		wait_for_process(pid, env);
	return (pid);
}

void	execute(t_ast_node *ast, t_env **env)
{
	t_ast_node	*node;

	if (!ast)
		return ;
	node = ast;
	if (node->type == TOKEN_WORD)
		execute_command(node, env);
	else if (node->type == TOKEN_REDIR_INPUT)
		execute_redir_input(node, env);
	else if (node->type == TOKEN_REDIR_OUTPUT)
		execute_redir_output(node, env);
	else if (node->type == TOKEN_REDIR_APPEND)
		execute_redir_append(node, env);
	else if (node->type == TOKEN_HERE_DOC)
		execute_here_doc(node, env);
	else if (node->type == TOKEN_BACKGROUND)
		execute_background(node, env);
	else if (node->type == TOKEN_PIPE)
		execute_pipe(node, env);
	else if (node->type == TOKEN_AND_IF)
		execute_and(node, env);
	else if (node->type == TOKEN_OR_IF)
		execute_or(node, env);
	else if (node->type == TOKEN_SEMI)
		execute_semi(node, env);
}

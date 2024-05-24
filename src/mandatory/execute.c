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

pid_t	execute_command(t_ast_node *node, t_env **env)
{
	pid_t	pid;

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

void	execute_redir_input(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(node->args[0], O_RDONLY);
	if (fd == -1)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
	if ((*env)->fd_in == -1)
		(*env)->fd_in = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	restore_origin_fd(origin_fd, env);
}

void	execute_redir_output(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
	if ((*env)->fd_out == -1)
		(*env)->fd_out = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	restore_origin_fd(origin_fd, env);
}

void	execute_redir_append(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		error_msg(NULL, errno);
		cleanup(env, EXIT_FAILURE);
	}
	if ((*env)->fd_out == -1)
		(*env)->fd_out = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	restore_origin_fd(origin_fd, env);
}

void	heredoc_output(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(".here_doc", O_RDONLY, 0644);
	if_error(fd == -1, env);
	if ((*env)->fd_in == -1)
		(*env)->fd_in = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	unlink(".here_doc");
	restore_origin_fd(origin_fd, env);
}

int here_doc_signal_handler(t_env **env, int fd, int *origin_fd)
{
	int	status;

	status = 0;
	close(fd);
	if (g_signal)
	{
		(*env)->exit_status = g_signal + 128;
		g_signal = 0;
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		status = 1;
	}
	restore_origin_fd(origin_fd, env);
	return (0);
}

void	execute_here_doc(t_ast_node *node, t_env **env)
{
	int		fd;
	int		origin_fd[2];
	char	*line;

	set_origin_fd(origin_fd);
	fd = open(".here_doc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if_error(fd == -1, env);
	dup2(0, STDOUT_FILENO);
	line = readline("> ");
	while (line)
	{
		if (ft_strcmp(line, node->args[0]) == 0 || g_signal)
		{
			ft_free_str(&line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		ft_free_str(&line);
		line = readline("> ");
	}
	if (here_doc_signal_handler(env, fd, origin_fd))
		return ;
	heredoc_output(node, env);
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

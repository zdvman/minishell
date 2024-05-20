/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:00:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/20 18:06:03 by dzuiev           ###   ########.fr       */
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

void execute_background(t_ast_node *node, t_env **env)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_perror("fork failed");
		cleanup(env, EXIT_FAILURE);
	}
	if (pid == 0)
	{
		execute(node->left, env);
		exit((*env)->exit_status);
	}
	else
	{
		waitpid(pid, &(*env)->exit_status, 0);
		(*env)->exit_status = WEXITSTATUS((*env)->exit_status);
	}
}

void execute_pipe(t_ast_node *node, t_env **env)
{
	int		fd[2];
	pid_t	pid;
	int	original_stdin;

	original_stdin = dup(STDIN_FILENO);
	handle_fd(env);
	if (pipe(fd) == -1)
	{
		ft_perror("pipe failed");
		cleanup(env, EXIT_FAILURE);
	}
	pid = fork();
	if (pid == -1)
	{
		ft_perror("fork failed");
		cleanup(env, EXIT_FAILURE);
	}
	if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		execute(node->left, env);
		exit((*env)->exit_status);
	}
	else
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		execute(node->right, env);
		dup2(original_stdin, STDIN_FILENO);
		close(original_stdin);
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
	path  = get_path(cmd, env);
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

void handle_fd(t_env **env)
{
	if ((*env)->fd_out != -1)
	{
		if (dup2((*env)->fd_out, STDOUT_FILENO) == -1)
		{
			ft_perror("dup2 failed");
			cleanup(env, EXIT_FAILURE);
		}
		close((*env)->fd_out);
		(*env)->fd_out = -1;
	}
	if ((*env)->fd_in != -1)
	{
		if (dup2((*env)->fd_in, STDIN_FILENO) == -1)
		{
			ft_perror("dup2 failed");
			cleanup(env, EXIT_FAILURE);
		}
		close((*env)->fd_in);
		(*env)->fd_in = -1;
	}
}

void execute_command(t_ast_node *node, t_env **env)
{
	pid_t	pid;

	// if (cmd_is_not_valid(node->args[0], env))
	// 	return ;
	handle_fd(env);
	if (is_builtin(node->args[0]))
	{
		execute_builtin(env, node->args);
		return ;
	}
	if (is_assignment(node->args[0]) && !node->args[1])
	{
		insert_local(*env, node->args[0]);
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		ft_perror("fork failed");
		cleanup(env, EXIT_FAILURE);
	}
	if (pid == 0)
	{
		if (is_assignment(node->args[0]))
		{
			insert_local(*env, node->args[0]);
			execve(get_path(node->args[1], env), &node->args[1], (*env)->envp);
		}
		else
			execve(get_path(node->args[0], env), node->args, (*env)->envp);
		ft_perror("execve fail");
		cleanup_loop(NULL, env);
		return ;
		// cleanup(env, EXIT_FAILURE);
	}
	else
	{
		waitpid(pid, &(*env)->exit_status, 0); // Ждем завершения дочернего процесса
		(*env)->exit_status = WEXITSTATUS((*env)->exit_status); // Получаем статус завершения дочернего процесса
	}
}

void execute_redir_input(t_ast_node *node, t_env **env)
{
	int	fd;
	int	original_stdin;

	original_stdin = dup(STDIN_FILENO);
	fd = open(node->args[0], O_RDONLY);
	if (fd == -1)
	{
		ft_perror("open failed");
		cleanup(env, EXIT_FAILURE);
	}
	// dup2(fd, STDIN_FILENO);
	if ((*env)->fd_in == -1)
		(*env)->fd_in = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	dup2(original_stdin, STDIN_FILENO);
	close(original_stdin);
}

void execute_redir_output(t_ast_node *node, t_env **env)
{
	int	fd;
	int	original_stdout;

	original_stdout = dup(STDOUT_FILENO);
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		ft_perror("open failed");
		cleanup(env, EXIT_FAILURE);
	}
	// dup2(fd, STDOUT_FILENO);
	if ((*env)->fd_out == -1)
		(*env)->fd_out = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdout);
}

void execute_redir_append(t_ast_node *node, t_env **env)
{
	int	fd;
	int	original_stdout;

	original_stdout = dup(STDOUT_FILENO);
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1) {
		ft_perror("open failed");
		cleanup(env, EXIT_FAILURE);
	}
	// dup2(fd, STDOUT_FILENO);
	if ((*env)->fd_out == -1)
		(*env)->fd_out = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdout);
}

void execute_here_doc(t_ast_node *node, t_env **env)
{
	int		fd;
	int		original_stdin;
	char	*line;

	fd = open(".here_doc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_perror("open failed");
		cleanup(env, EXIT_FAILURE);
	}
	line = readline("> ");
	while (line)
	{
		if (ft_strcmp(line, node->args[0]) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
		line = readline("> ");
	}
	close(fd);
	original_stdin = dup(STDIN_FILENO);
	fd = open(".here_doc", O_RDONLY, 0644);
	if (fd == -1)
	{
		ft_perror("open failed");
		cleanup(env, EXIT_FAILURE);
	}
	// dup2(fd, STDIN_FILENO);
	if ((*env)->fd_in == -1)
		(*env)->fd_in = fd;
	if (node->left)
		execute(node->left, env);
	close(fd);
	unlink(".here_doc");
	dup2(original_stdin, STDIN_FILENO);
	close(original_stdin);
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

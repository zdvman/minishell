/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 12:38:08 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/28 12:38:08 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	execute_redir_output(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		error_msg(node->args[0], errno);
		cleanup_no_exit(env);
	}
	if ((*env)->fd_out == -1)
		(*env)->fd_out = fd;
	if (node->left)
		execute(node->left, env);
	else
	{
		(*env)->fd_in = -1;
		(*env)->fd_out = -1;
	}
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
		error_msg(node->args[0], errno);
		cleanup_no_exit(env);
	}
	if ((*env)->fd_out == -1)
		(*env)->fd_out = fd;
	if (node->left)
		execute(node->left, env);
	else
	{
		(*env)->fd_in = -1;
		(*env)->fd_out = -1;
	}
	close(fd);
	restore_origin_fd(origin_fd, env);
}

void	execute_redir_input(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(node->args[0], O_RDONLY);
	if (fd == -1)
	{
		error_msg(node->args[0], errno);
		cleanup_no_exit(env);
	}
	if ((*env)->fd_in == -1)
		(*env)->fd_in = fd;
	if (node->left)
		execute(node->left, env);
	else
	{
		(*env)->fd_in = -1;
		(*env)->fd_out = -1;
	}
	close(fd);
	restore_origin_fd(origin_fd, env);
}

static void	heredoc_output(t_ast_node *node, t_env **env)
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
	else
	{
		(*env)->fd_in = -1;
		(*env)->fd_out = -1;
	}
	close(fd);
	unlink(".here_doc");
	restore_origin_fd(origin_fd, env);
}

void	execute_here_doc(t_ast_node *node, t_env **env)
{
	int		fd;
	int		origin_fd[2];
	char	*line;

	set_origin_fd(origin_fd);
	fd = open(".here_doc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if_error(fd == -1, env);
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

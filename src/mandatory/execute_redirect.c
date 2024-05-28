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

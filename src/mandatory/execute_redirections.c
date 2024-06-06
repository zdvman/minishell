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
	if (check_for_wildcard(env, &node->args[0]))
	{
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		error_msg(node->args[0], errno);
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	dup2(fd, STDOUT_FILENO);
	if (node->left)
		execute(node->left, env);
	restore_origin_fd(origin_fd, env);
	close(fd);
}

void	execute_redir_append(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	if (check_for_wildcard(env, &node->args[0]))
	{
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	fd = open(node->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		error_msg(node->args[0], errno);
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	dup2(fd, STDOUT_FILENO);
	if (node->left)
		execute(node->left, env);
	restore_origin_fd(origin_fd, env);
	close(fd);
}

void	execute_redir_input(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	if (check_for_wildcard(env, &node->args[0]))
	{
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	fd = open(node->args[0], O_RDONLY);
	if (fd == -1)
	{
		error_msg(node->args[0], errno);
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	dup2(fd, STDIN_FILENO);
	if (node->left)
		execute(node->left, env);
	restore_origin_fd(origin_fd, env);
	close(fd);
}

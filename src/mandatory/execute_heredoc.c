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

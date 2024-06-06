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

void	free_list(t_list **list)
{
	t_list	*tmp;

	while (*list)
	{
		tmp = *list;
		free ((*list)->content);
		*list = (*list)->next;
		free (tmp);
	}
}

int	check_for_wildcard(t_env **env, char **input)
{
	t_list	*args;

	if (!contains(*input, '*'))
		return (0);
	get_current_dir(env);
	args = expand_args(env, *input, (*env)->tokens);
	free_dir(env);
	if (!args)
	{
		return (put_3("minshell: ", *input,
				": No such file or directory\n"), 1);
	}
	if (args && args->next)
	{
		free_list (&args);
		return (put_3("minshell: ", *input, ": Ambiguous redirect\n"), 1);
	}
	free (*input);
	*input = ft_strdup(args->content);
	free_list (&args);
	return (0);
}

static void	heredoc_output(t_ast_node *node, t_env **env)
{
	int	fd;
	int	origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(".here_doc", O_RDONLY, 0644);
	if (fd == -1)
	{
		error_msg(".here_doc", errno);
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	dup2(fd, STDIN_FILENO);
	if (node->left)
		execute(node->left, env);
	restore_origin_fd(origin_fd, env);
	close(fd);
	unlink(".here_doc");
}

void	get_here_doc_input(t_env **env, t_ast_node *node, int fd)
{
	char	*line;

	dup2((*env)->stdout, STDOUT_FILENO);
	line = readline("> ");
	while (line)
	{
		signal(SIGQUIT, SIG_DFL);
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
}

void	execute_here_doc(t_ast_node *node, t_env **env)
{
	int		fd;
	int		origin_fd[2];

	set_origin_fd(origin_fd);
	fd = open(".here_doc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		error_msg(".here_doc", errno);
		restore_origin_fd(origin_fd, env);
		(*env)->exit_status = 1;
		return ;
	}
	get_here_doc_input(env, node, fd);
	if (here_doc_signal_handler(env, fd, origin_fd))
		return ;
	restore_origin_fd(origin_fd, env);
	close(fd);
	heredoc_output(node, env);
}

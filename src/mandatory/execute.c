/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 12:50:26 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 13:32:53 by dzuiev           ###   ########.fr       */
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

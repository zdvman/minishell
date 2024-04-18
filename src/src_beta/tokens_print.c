/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:42:04 by samoore           #+#    #+#             */
/*   Updated: 2024/04/18 14:42:08 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_tokens(t_token *tokens)
{
	while (tokens)
	{
		if (tokens->type == WORD)
			printf("WORD: %s\n", tokens->string);
		if (tokens->type == REDIR_IN)
			printf("REDIR_IN\n");
		if (tokens->type == REDIR_IN_DBL)
			printf("REDIR_IN_DBL\n");
		if (tokens->type == REDIR_OUT)
			printf("REDIR_OUT\n");
		if (tokens->type == REDIR_OUT_DBL)
			printf("REDIR_OUT_DBL\n");
		if (tokens->type == PIPE)
			printf("PIPE\n");
		if (tokens->type == CONTROL)
			printf("CONTROL\n");
		if (tokens->type == DONE)
		{
			printf("DONE\n");
			printf("exit status: %d\n", tokens->val);
		}
		if (tokens->type == EXIT)
			printf("EXIT\n");
		if (tokens->type == OPEN_BRACKET)
			printf("OPEN_BRACKET\n");
		if (tokens->type == CLOSE_BRACKET)
			printf("CLOSE_BRACKET\n");
		if (tokens->type == COMMAND)
		{
			printf("COMMAND\n");
			int i = 0;
			while (tokens->args[i])
			{
				printf("arg %d: %s\n", i, tokens->args[i]);
				i++;
			}
			tokens->redirects = tokens->redirects_head;
			while (tokens->redirects)
			{
				printf("redirct %d: %s\n", tokens->redirects->type, tokens->redirects->file);
				tokens->redirects = tokens->redirects->next;
			}
			printf("path %s\n", tokens->command_path);
		}
		if (tokens->type == END)
			printf("END\n");
		tokens = tokens->right;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_visualizer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:10:32 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 14:18:54 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
#include "../includes/minishell.h"

void	print_escaped(FILE *stream, const char *str)
{
	if (!str)
		return ;
	while (*str)
	{
		switch (*str)
		{
			case '\n': fprintf(stream, "\\n"); break;
			case '\"': fprintf(stream, "\\\""); break;
			case '\\': fprintf(stream, "\\\\"); break;
			default: fputc(*str, stream);
		}
		str++;
	}
}

void print_ast_dot(t_ast_node *node, FILE *stream)
{
	if (node == NULL) return;

	fprintf(stream, "\"%p\" [label=\"", (void*)node);
	switch (node->type)
	{
		case TOKEN_WORD: fprintf(stream, "CMD: "); break;
		case TOKEN_REDIR_INPUT:
		case TOKEN_REDIR_OUTPUT:
		case TOKEN_REDIR_APPEND:
		case TOKEN_HERE_DOC: fprintf(stream, "REDIR: "); break;
		case TOKEN_PIPE: fprintf(stream, "|"); break;
		case TOKEN_BACKGROUND: fprintf(stream, "&"); break;
		case TOKEN_AND_IF: fprintf(stream, "&&"); break;
		case TOKEN_OR_IF: fprintf(stream, "||"); break;
		case TOKEN_SEMI: fprintf(stream, ";"); break;
		break;
		default: fprintf(stream, "UNKNOWN"); break;
	}
	if (node->args)
	{
		for (int i = 0; node->args[i] != NULL; i++)
		{
			if (i > 0) fprintf(stream, " "); // Add space between arguments
			print_escaped(stream, node->args[i]);
		}
	}
	fprintf(stream, "\"];\n");
	if (node->left != NULL)
	{
		fprintf(stream, "\"%p\" -> \"%p\" [label=\"L\"];\n",
				(void*)node, (void*)node->left);
		print_ast_dot(node->left, stream);
	}
	if (node->right != NULL)
	{
		fprintf(stream, "\"%p\" -> \"%p\" [label=\"R\"];\n",
				(void*)node, (void*)node->right);
		print_ast_dot(node->right, stream);
	}
}

void	generate_ast_diagram(t_ast_node *root)
{
	(void)root;
	FILE *stream = fopen("ast.dot", "w");
	if (stream == NULL)
	{
		perror("fopen");
		return;
	}
	fprintf(stream, "digraph AST {\n");
	print_ast_dot(root, stream);
	fprintf(stream, "}\n");
	fclose(stream);
}
*/

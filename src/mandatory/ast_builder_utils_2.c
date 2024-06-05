/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder_utils_2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 12:41:58 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/27 12:47:36 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

bool	is_redirection(t_token_type type)
{
	if (type == TOKEN_REDIR_INPUT
		|| type == TOKEN_REDIR_OUTPUT
		|| type == TOKEN_REDIR_APPEND
		|| type == TOKEN_HERE_DOC)
		return (true);
	else
		return (false);
}

bool	is_control_op(t_token_type type)
{
	if (type == TOKEN_AND_IF
		|| type == TOKEN_OR_IF
		|| type == TOKEN_SEMI
		|| type == TOKEN_PIPE
		|| type == TOKEN_BACKGROUND)
		return (true);
	else
		return (false);
}

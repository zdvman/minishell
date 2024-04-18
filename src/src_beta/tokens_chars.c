/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_chars.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:37:26 by samoore           #+#    #+#             */
/*   Updated: 2024/04/18 14:37:44 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirect(char c)
{
	if (c == '<' || c == '>')
		return (1);
	return (0);
}

int	is_control(char c)
{
	if (c == '|' || c == '&')
		return (1);
	return (0);
}

int	is_bracket(char c)
{
	if (c == '(')
		return (1);
	if (c == ')')
		return (2);
	return (0);
}


int	is_quote(char c)
{
	if (c == '\'' || c == '\"')
		return (1);
	return (0);
}

int	is_meta(char c)
{
	if (is_control(c) || is_redirect(c) || is_space(c) || is_bracket(c))
		return (1);
	if (is_quote(c))
		return (1);
	if (c == 0)
		return (1);
	return (0);
}

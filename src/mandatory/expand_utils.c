/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:04:10 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 19:30:33 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	star_check_in_token(char *token_value)
{
	char	*value;
	char	current_quote;

	value = token_value;
	current_quote = 0;
	if (contains(value, '*'))
	{
		while (*value)
		{
			current_quote = is_quote_open(value);
			if (*value == '\\'
				&& *(value + 1) == '*'
				&& !current_quote)
			{
				value += 2;
				continue ;
			}
			if (*value == '*' && !current_quote)
				return (1);
			value++;
		}
	}
	return (0);
}

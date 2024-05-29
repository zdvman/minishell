/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 11:59:32 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/29 11:49:09 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	cleanup_loop(char **input, t_env **env)
{
	if (input && *input)
		ft_free_str(input);
	ft_free_tokens(&(*env)->tokens);
	ft_free_ast(&(*env)->ast);
}

void	cleanup_no_exit(t_env **env)
{
	ft_free_tokens(&(*env)->tokens);
	ft_free_ast(&(*env)->ast);
}

void	ft_free_args(char ***args)
{
	int	i;

	if (args == NULL || *args == NULL)
		return ;
	i = 0;
	while ((*args)[i])
	{
		free((*args)[i]);
		(*args)[i] = NULL;
		i++;
	}
	free(*args);
	*args = NULL;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_memory.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:33:47 by samoore           #+#    #+#             */
/*   Updated: 2024/04/18 14:33:49 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	reset_tokens(t_env *env)
{
	t_token	*tmp;
	int		i;

	while (env->tokens->type != END)
	{
		if (env->tokens->command_path)
		{
			free (env->tokens->command_path);
			env->tokens->command_path = NULL;
		}
		if (env->tokens->string)
		{
			free (env->tokens->string);
			env->tokens->string = NULL;
		}
		i = 0;
		if (env->tokens->args)
		{
			while (env->tokens->args[i])
				free (env->tokens->args[i++]);
			free (env->tokens->args);
			env->tokens->args = NULL;
		}
		tmp = env->tokens->right;
		free (env->tokens);
		env->tokens = tmp;
	}
	free (env->tokens);
	env->tokens = NULL;
}

void	clear_env(t_env *env)
{
	int	i;

	reset_tokens(env);
	free_paths(env);
	//free (env->paths);
}


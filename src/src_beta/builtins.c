/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 12:07:54 by samoore           #+#    #+#             */
/*   Updated: 2024/04/24 12:08:06 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	change_dir(t_env *env)
{
	t_redirect	*tmp;

	if (env->tokens->redirects_head)
	{
		tmp = env->tokens->redirects_head;
		while (tmp)
		{
			open_redirect_file(tmp);
			tmp = tmp->next;
		}
	}
	return (chdir(env->tokens->args[1]));
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 18:42:01 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/11 14:28:59 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	env_init(t_env **env, char **envp, int env_len)
{
	int	i;

	(*env)->envp = malloc(sizeof(char *) * (env_len + 1));
	if ((*env)->envp == NULL)
	{
		ft_perror("malloc failed\n");
		cleanup(env, EXIT_FAILURE);
	}
	(*env)->envp[env_len] = NULL;
	i = -1;
	while (envp[++i] != NULL)
	{
		(*env)->envp[i] = ft_strdup(envp[i]);
		if ((*env)->envp[i] == NULL)
		{
			ft_perror("malloc failed\n");
			cleanup(env, EXIT_FAILURE);
		}
	}
	(*env)->tokens = NULL;
	(*env)->head_token = NULL;
	(*env)->ast = NULL;
	(*env)->exit_status = 0;
	(*env)->pipe_fd[0] = -1;
	(*env)->pipe_fd[1] = -1;
	(*env)->fd_in = -1;
	(*env)->fd_out = -1;
}

void	shell_init(t_env **env, char **envp)
{
	int	env_len;

	set_sig_actions();
	printf("\033[?12h");
	*env = malloc(sizeof(t_env));
	if (*env == NULL)
	{
		ft_perror("malloc failed\n");
		cleanup(env, EXIT_FAILURE);
	}
	env_len = ft_strarr_len(envp);
	env_init(env, envp, env_len);
}

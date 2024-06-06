/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 18:42:01 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 19:16:26 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	env_init_2(t_env **env)
{
	(*env)->syntax_error = 0;
	(*env)->exit_status = 0;
	(*env)->pipe_fd[0] = -1;
	(*env)->pipe_fd[1] = -1;
	(*env)->fd_in = -1;
	(*env)->fd_out = -1;
	(*env)->ast = NULL;
	(*env)->tokens = NULL;
	(*env)->head_token = NULL;
	(*env)->directory_list = NULL;
	(*env)->dir_head = NULL;
	(*env)->loc_vars = NULL;
	(*env)->user_host = NULL;
	(*env)->prompt = NULL;
	(*env)->stdout = dup(STDOUT_FILENO);
	(*env)->stdin = dup(STDIN_FILENO);
	(*env)->stderr = dup(STDERR_FILENO);
}

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
	env_init_2(env);
	get_host_and_user(*env);
}

void	shell_init(t_env **env, char **envp)
{
	int	env_len;

	(void)envp;
	set_sig_actions();
	printf("\033[?12h");
	*env = ft_calloc(sizeof(t_env), 1);
	if (*env == NULL)
	{
		ft_perror("malloc failed\n");
		cleanup(env, EXIT_FAILURE);
	}
	env_len = ft_strarr_len(envp);
	env_init(env, envp, env_len);
	(*env)->loc_vars = ft_calloc(sizeof(char *), 1);
	(*env)->prompt = NULL;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:30:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/29 17:37:44 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_global	g_global;

static void	signal_handler(int signal)
{
	pid_t	pid[MAX_CHILDREN];
	int		pid_count;

	ft_memcpy(pid, g_global.pid, sizeof(pid_t) * MAX_CHILDREN);
	pid_count = g_global.pid_count;
	g_global.g_signal = signal;
	if (pid_count > 0)
	{
		while (pid_count > 0)
			kill(pid[--pid_count], SIGTERM);
		printf("in_child\n");
	}
	else
	{
		ioctl(STDIN_FILENO, TIOCSTI, "\n");
		rl_redisplay();
		rl_on_new_line();
		rl_replace_line("", 0);
	}
	(void)signal;
}

void	set_sig_actions(void)
{
	struct sigaction	sa;
	struct sigaction	ignore;

	sa.sa_handler = (void *)signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	ignore.sa_handler = SIG_IGN;
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &ignore, NULL);
}

void	exit_minishell(t_env **env)
{
	write(1, "exit\n", 5);
	cleanup(env, (*env)->exit_status);
}

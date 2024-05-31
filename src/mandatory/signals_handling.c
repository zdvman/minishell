/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:30:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/30 11:48:24 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// t_global	g_global;

volatile sig_atomic_t	g_signal;

static void	signal_handler(int signal)
{
	g_signal = signal;
	if (!pid_list(GET_NUM, 0))
	{
		ioctl(STDIN_FILENO, TIOCSTI, "\n");
		rl_redisplay();
		rl_on_new_line();
		rl_replace_line("", 0);
	}
	else
	{
		pid_list(KILL, 0);
		set_sig_actions ();
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

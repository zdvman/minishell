/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 11:26:28 by dzuiev            #+#    #+#             */
/*   Updated: 2024/04/15 11:26:28 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

volatile sig_atomic_t g_sigint_received = 0;

void handle_sigint(int sig)
{
	(void)sig;
	g_sigint_received = 1;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_set_prompt("minishell> ");
	rl_redisplay();
}

void handle_sigquit(int sig)
{
	(void)sig; // Unused parameter
	// Do nothing
}

void set_sig_actions(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	// rl_catch_signals = 0; 
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);

	sa_quit.sa_handler = handle_sigquit;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

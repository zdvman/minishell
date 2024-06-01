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

int	print_cwd(char *args)
{
	char	cwd[256];

	if (args && args[0] == '-' && args[1])
	{
		ft_putstr("minishell: pwd: ");
		ft_putchar(args[0]);
		ft_putchar(args[1]);
		ft_putstr(": invalid option\npwd: usage: pwd\n");
		return (2);
	}
	getcwd(cwd, 256);
	ft_putstr(cwd);
	ft_putchar('\n');
	return (0);
}

int	print_env(t_env *env, char *args)
{
	int	i;

	i = 0;
	if (args)
	{
		ft_putstr("minishell: env: ");
		ft_putstr(args);
		ft_putstr(": invalid option\nenv: usage: env\n");
		return (125);
	}
	while (env->envp && env->envp[i])
	{
		ft_putstr(env->envp[i++]);
		ft_putchar('\n');
	}
	return (0);
}

int	echo(char **args)
{
	int	i;
	int	n;

	i = 1;
	if (!args[i])
	{
		ft_putchar('\n');
		return (0);
	}
	n = (!ft_strcmp(args[i], "-n"));
	while (args[i + n])
	{
		ft_putstr(args[i++ + n]);
		if (args[i + n])
			ft_putchar(' ');
	}
	if (!n)
		ft_putchar('\n');
	return (0);
}

int	clean_exit(t_env **env, char **args)
{
	int	code;

	code = EXIT_SUCCESS;
	if (args[1] && ft_isnumber(args[1]) && args[2])
	{
		ft_putstr("minishell: exit: too many arguments\n");
		return (1);
	}
	ft_putstr("exit\n");
	if (args[1] && !ft_isnumber(args[1]))
	{
		ft_putstr("minishell: exit: ");
		ft_putstr(args[1]);
		ft_putstr(": numeric argument required\n");
		if ((*env)->input && *(*env)->input)
			free ((*(*env)->input));
		cleanup(env, 0);
		exit (2);
	}
	if (args[1])
		code = ft_atoi(args[1]);
	if ((*env)->input && *(*env)->input)
		free ((*(*env)->input));
	return (cleanup(env, code), 0);
}

void	execute_builtin(t_env **env, char **args)
{
	int	exit;

	exit = 0;
	if (!ft_strcmp(args[0], "cd") && args[1] && args[1][0])
		exit = change_dir(env, args);
	else if (!ft_strcmp(args[0], "pwd"))
		exit = print_cwd(args[1]);
	else if (!ft_strcmp(args[0], "env"))
		exit = print_env(*env, args[1]);
	else if (!ft_strcmp(args[0], "echo"))
		exit = echo(args);
	else if (!ft_strcmp(args[0], "exit"))
		exit = clean_exit(env, args);
	else if (!ft_strcmp(args[0], "export"))
		exit = export_var(*env, args);
	else if (!ft_strcmp(args[0], "unset"))
	{
		remove_var(args[1], &(*env)->envp);
		remove_var(args[1], &(*env)->loc_vars);
	}
	(*env)->exit_status = exit;
}

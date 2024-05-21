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

int	is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "echo"))
		return (1);
	if (!ft_strcmp(cmd, "cd"))
		return (1);
	if (!ft_strcmp(cmd, "pwd"))
		return (1);
	if (!ft_strcmp(cmd, "env"))
		return (1);
	if (!ft_strcmp(cmd, "unset"))
		return (1);
	if (!ft_strcmp(cmd, "exit"))
		return (1);
	if (!ft_strcmp(cmd, "export"))
		return (1);
	return (0);
}

void	change_dir(t_env **env, char **args)
{
	char	*tmp;
	char	cwd[256];
	char	old_cwd[256];

	tmp = NULL;
	if (args[2])
		return ((void)ft_putstr("minishell: cd: too many arguments\n"));
	if (args[1][0] == '~')
		tmp = ft_strjoin(get_env_variable(*env, "HOME"), &args[1][1]);
	else
		tmp = ft_strdup(args[1]);
	if (access(tmp, R_OK))
		return ((void)ft_putstr("minishell: cd: "), (void)ft_putstr(args[1]),
				(void)ft_putstr(": No such file or directory\n"));
	free (args[1]);
	args[1] = tmp;
	getcwd(old_cwd, 256);
	if ((chdir(args[1])) == -1)
		return ;
	getcwd(cwd, 256);
	replace_or_add_env_var(env, "OLDPWD", old_cwd);
	replace_or_add_env_var(env, "PWD", cwd);
}

void	print_cwd(void)
{
	char	cwd[256];

	getcwd(cwd, 256);
	ft_putstr(cwd);
	ft_putchar('\n');
}

void	print_env(t_env *env)
{
	int	i;

	i = 0;
	while (env->envp[i])
	{
		ft_putstr(env->envp[i++]);
		ft_putchar('\n');
	}
}

void	echo(char **args)
{
	int	i;
	int	n;

	i = 1;
	n = (!ft_strcmp(args[i], "-n"));
	while (args[i + n])
	{
		ft_putstr(args[i++ + n]);
		if (args[i + n])
			ft_putchar(' ');
	}
	if (!n)
		ft_putchar('\n');
}

int	is_assignment(char *cmd)
{
	int	i;

	i = 0;
	if (cmd[0] != '_' && !ft_isalpha(cmd[0]))
		return (0);
	i++;
	while (cmd[i] && cmd[i] != '=')
	{
		if (cmd[0] != '_' && !ft_isalnum(cmd[0]))
			return (0);
		i++;
	}
	if (cmd[i] && cmd[i] == '=')
		return (1);
	return (0);
}

void	clean_exit(t_env **env, char **args)
{
	int	code;

	code = EXIT_SUCCESS;
	if (args[1] && ft_isnumber(args[1]) && args[2])
	{
		ft_putstr("minishell: exit: too many arguments\n");
		return ;
	}
	ft_putstr("exit\n");
	if (args[1] && !ft_isnumber(args[1]))
	{
		ft_putstr("minishell: exit: ");
		ft_putstr(args[1]);
		ft_putstr(": numeric argument required\n");
		cleanup(env, 0);
		exit (2);
	}
	if (args[1])
		code = ft_atoi(args[1]);
	cleanup(env, 0);
	exit (code);
}

void	execute_builtin(t_env **env, char **args)
{
	if (!ft_strcmp(args[0], "cd") && args[1][0])
		change_dir(env, args);
	if (!ft_strcmp(args[0], "pwd"))
		print_cwd();
	if (!ft_strcmp(args[0], "env"))
		print_env(*env);
	if (!ft_strcmp(args[0], "echo"))
		echo(args);
	if (!ft_strcmp(args[0], "exit"))
		clean_exit(env, args);
	if (!ft_strcmp(args[0], "export"))
		export_var(*env, args);
	if (!ft_strcmp(args[0], "unset"))
		replace_or_add_env_var(env, args[1], NULL);
}

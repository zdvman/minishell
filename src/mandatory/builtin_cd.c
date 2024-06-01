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

static int	handle_dash(t_env **env, char **args)
{
	if (!args[1][1])
	{
		free (args[1]);
		args[1] = ft_strdup(get_var("OLDPWD", (*env)->envp));
		return (change_dir(env, args));
	}
	args[1][2] = 0;
	put_3("minishell: cd: ", args[1],
		": invalid option\ncd: usage: cd [-] [dir]\n");
	return (2);
}

static int	check_access(char *tmp, char **args)
{
	if (access(tmp, F_OK))
		return (put_3("minishell: cd: ", args[1],
				": No such file or directory\n"), 1);
	if (access(tmp, R_OK | X_OK))
		return (put_3("minishell: cd: ", args[1],
				": Permission denied\n"), 1);
	return (0);
}

static void	update_env(t_env **env, char *old_cwd, char **tmp)
{
	char	cwd[256];

	getcwd(cwd, 256);
	remove_var("OLDPWD", &(*env)->envp);
	remove_var("PWD", &(*env)->envp);
	*tmp = ft_strjoin("OLDPWD=", old_cwd);
	add_var(*tmp, &(*env)->envp);
	free (*tmp);
	*tmp = ft_strjoin("PWD=", cwd);
	add_var(*tmp, &(*env)->envp);
}

int	change_dir(t_env **env, char **args)
{
	char	*tmp;
	char	old_cwd[256];

	tmp = NULL;
	if (!args[1])
		return (1);
	if (args[1][0] == '-')
		return (handle_dash(env, args));
	if (args[1] && args[2])
		return ((void)ft_putstr("minishell: cd: too many arguments\n"), 1);
	if (args[1][0] == '~')
		tmp = ft_strjoin(get_var("HOME", (*env)->envp), &args[1][1]);
	else
		tmp = ft_strdup(args[1]);
	if (check_access(tmp, args))
		return (free (tmp), 1);
	free (args[1]);
	args[1] = tmp;
	getcwd(old_cwd, 256);
	if ((chdir(args[1])) == -1)
		return (free (tmp), 1);
	update_env(env, old_cwd, &tmp);
	free (tmp);
	return (0);
}

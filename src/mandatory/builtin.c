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

char	*make_var(char *var, char *val)
{
	char	*tmp;
	char	*res;

	tmp = ft_strjoin(var, "=");
	res = ft_strjoin(tmp, val);
	free (tmp);
	return (res);
}

void	remove_var(t_env *env, int target)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->envp[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count);
	while (i < target)
	{
		new_env[i] = env->envp[i];
		i++;
	}
	while (i < count - 1)
	{
		new_env[i] = env->envp[i + 1];
		i++;
	}
	// free (env->envp[target]);
	free (env->envp);
	env->envp = new_env;
}

void	add_var(t_env *env, char *var, char *val)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (env->envp[count])
		count++;
	new_env = ft_calloc(sizeof(char *), count + 2);
	while (i < count)
	{
		new_env[i] = env->envp[i];
		i++;
	}
	new_env[i++] = make_var(var, val);
	// free (env->envp[target]);
	free (env->envp);
	env->envp = new_env;
}

void	replace_env_var(t_env **env, char *var, char *val)
{
	int		i;

	i = 0;
	while ((*env)->envp[i])
	{
		if (!ft_strncmp((*env)->envp[i], var, ft_strlen(var)))
		{
			free ((*env)->envp[i]);
			if (val)
				(*env)->envp[i] = make_var(var, val);
			else
				remove_var(*env, i);
			return ;
		}
		i++;
	}
}

void	replace_or_add_env_var(t_env **env, char *var, char *val)
{
	int		i;

	i = 0;
	while ((*env)->envp[i])
	{
		if (!ft_strncmp((*env)->envp[i], var, ft_strlen(var)))
		{
			free ((*env)->envp[i]);
			if (val && *val)
				(*env)->envp[i] = make_var(var, val);
			else
				remove_var(*env, i);
			return ;
		}
		i++;
		if (!(*env)->envp[i])
			add_var(*env, var, val);
	}
}

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
	return (0);
}

char	*get_env_variable(t_env *env, char *env_var)
{
	int		i;

	i = 0;
	while (env->envp[i])
	{
		if (!ft_strncmp(env->envp[i], env_var, ft_strlen(env_var)))
			return (&env->envp[i][ft_strlen(env_var) + 1]);
		i++;
	}
	return (NULL);
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
	replace_env_var(env, "OLDPWD", old_cwd);
	replace_env_var(env, "PWD", cwd);
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
		if (args[i])
			ft_putchar(' ');
	}
	if (!n)
		ft_putchar('\n');
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
	if (!ft_strcmp(args[0], "unset"))
		replace_env_var(env, args[1], NULL);
}

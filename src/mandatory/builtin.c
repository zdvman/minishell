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
	if (!ft_strcmp(cmd, "cd"))
		return (1);
	if (!ft_strcmp(cmd, "pwd"))
		return (1);
	return (0);
}

char	*get_env_variable(t_env *env, char *env_var)
{
	char	**env_entries;
	int		i;

	i = 0;
	env_entries = env->envp;
	while (env_entries[i])
	{
		if (!ft_strncmp(env_entries[i], env_var, ft_strlen(env_var)))
			return (&env_entries[i][ft_strlen(env_var) + 1]);
		i++;
	}
	return (NULL);
}

void	change_dir(t_env **env, char **args)
{
	char	*tmp;

	if (args[1][0] == '~')
	{
		tmp = ft_strjoin(get_env_variable(*env, "HOME"), &args[1][1]);
		free (args[1]);
		args[1] = tmp;
	}
	chdir(args[1]);
}

void	print_cwd(void)
{
	char	cwd[256];

	getcwd(cwd, 256);
	ft_putstr(cwd);
	ft_putchar('\n');
}

void	execute_builtin(t_env **env, char **args)
{
	if (!ft_strcmp(args[0], "cd"))
		change_dir(env, args);
	if (!ft_strcmp(args[0], "pwd"))
		print_cwd();
}

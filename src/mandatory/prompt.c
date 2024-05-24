/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:00:30 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/21 19:12:49 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	get_host_and_user(t_env *env)
{
	char	*res;
	char	buf[256];
	int		fd;
	int		i;

	i = 0;
	fd = open("/etc/hostname", O_RDONLY);
	if (fd == -1)
		return ;
	read(fd, buf, 255);
	close(fd);
	res = ft_calloc(1, 253);
	res[0] = '@';
	while (buf[i] && buf[i] != '.')
	{
		res[i + 1] = buf[i];
		i++;
	}
	res[i + 1] = ':';
	env->user_host = ft_strjoin(get_var("USER", env->envp), res);
	free (res);
}

int	valid_path(char *envp, char *loc)
{
	if (!envp && !loc)
		return (0);

	if (envp && envp[ft_strlen(envp) - 1] == '/')
		return (0);
	if (loc && loc[ft_strlen(loc) - 1] == '/')
		return (0);	 
	return (1);
}

char	*add_cwd(t_env *env, char *user_host)
{
	char	dir_name[256];
	char	*res;
	char	*tmp;
	char	*var;

	getcwd(dir_name, 256);
	if (!valid_path(get_var("HOME", env->envp), get_var("HOME", env->loc_vars)))
	{
		res = ft_strjoin(user_host, dir_name);
		return (res);
	}
	if (get_var("HOME", env->envp))
		var = (get_var("HOME", env->envp));
	else
		var = (get_var("HOME", env->loc_vars));
	if (ft_strncmp(dir_name, var, ft_strlen(var)))
	{
		res = ft_strjoin(user_host, dir_name);
		return (res);
	}
	tmp = ft_strjoin(user_host, "~");
	res = ft_strjoin(tmp, &dir_name[ft_strlen(var)]);
	free (tmp);
	return (res);
}

char	*prompt(t_env *env)
{
	char	*tmp;
	char	*tmp2;

	if (env->prompt)
	{
		free (env->prompt);
		env->prompt = NULL;
	}
	tmp = add_cwd(env, env->user_host);
	tmp2 = ft_strjoin(tmp, "$ ");
	free (tmp);
	env->prompt = tmp2;
	return (env->prompt);
}
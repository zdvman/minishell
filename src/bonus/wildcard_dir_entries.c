/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 11:53:04 by dzuiev            #+#    #+#             */
/*   Updated: 2024/04/10 12:01:34 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell_bonus.h"

t_list	*new_dir_entry(char *entry_name)
{
	t_list	*new;

	new = ft_calloc(sizeof(t_list), 1);
	new->content = ft_strdup(entry_name);
	return (new);
}

void	free_dir(t_env **env)
{
	t_list	*tmp;

	(*env)->directory_list = (*env)->dir_head;
	while ((*env)->directory_list)
	{
		tmp = (*env)->directory_list;
		free ((*env)->directory_list->content);
		(*env)->directory_list = (*env)->directory_list->next;
		free (tmp);
	}
	(*env)->directory_list = NULL;
	(*env)->dir_head = NULL;
}

void	get_current_dir(t_env **env)
{
	struct dirent	*entry;
	char			*dir_name;
	DIR				*current;

	(*env)->dir_head = NULL;
	dir_name = malloc(256);
	dir_name = getcwd(dir_name, 256);
	current = opendir(dir_name);
	free (dir_name);
	entry = readdir(current);
	while ((entry))
	{
		if (!(*env)->dir_head)
		{
			(*env)->directory_list = new_dir_entry(entry->d_name);
			(*env)->dir_head = (*env)->directory_list;
		}
		else
		{
			(*env)->directory_list->next = new_dir_entry(entry->d_name);
			(*env)->directory_list = (*env)->directory_list->next;
		}
		entry = readdir(current);
	}
	closedir(current);
}

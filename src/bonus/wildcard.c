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

int	good_pattern(char *pattern, char *file_name, int i, int j)
{
	if (pattern[i] == 0 || file_name[j] == 0)
		return (0);
	if (pattern[i] != '*' && pattern[i] != file_name[j])
		return (0);
	if (pattern[0] == '*' && file_name[0] == '.')
		return(0);
	if (pattern[0] == '*' && pattern[1] == '.' && file_name[0] == '.')
		return(0);
	return (1);	
}

int	glob(char *pattern, char *file_name, int i, int j)
{
	if (pattern[i] == 0 && file_name[j] == 0)
		return (1);
	if (file_name[j] == 0 && pattern[i] == '*')
		return (glob(pattern, file_name, i + 1, j));
	if (!good_pattern(pattern, file_name, i , j))
		return (0);
	if (pattern[0] == '.' && pattern[1] == '*'
		&& !pattern[2] && file_name[0] == '.')
		return (1);
	if (pattern[i] == '*')
		return (glob(pattern, file_name, i + 1, j + 1)
			|| glob(pattern, file_name, i, j + 1)
			|| glob(pattern, file_name, i + 1, j));
	else
		return (glob(pattern, file_name, i + 1, j + 1));
	return (0);
}

int	contains(char *str, char target)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == target)
			return (1);
		i++;
	}
	return (0);
}

t_list	*expand_args(t_env **env, char *pattern)
{
	t_list	*matched;
	t_list	*matched_head;

	matched = NULL;
	matched_head = NULL;
	(*env)->directory_list = (*env)->dir_head;
	while ((*env)->directory_list)
	{
		if (glob(pattern, (*env)->directory_list->content, 0, 0))
		{
			if (!matched)
			{
				matched = new_dir_entry((*env)->directory_list->content);
				matched_head = matched;
			}
			else
			{
				matched->next = new_dir_entry((*env)->directory_list->content);
				matched = matched->next;
			}
		}
		(*env)->directory_list = (*env)->directory_list->next;
	}

	return (matched_head);
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
/*
void	process_wildcard(t_env *env, t_list *prev)
{
	t_list	*next;
	t_list	*tmp;
	t_list	*get_args;

	get_current_dir(env);
	next = (*env)->tokens->args_list->next;
	tmp = (*env)->tokens->args_list;
	get_args = expand_args(env, (*env)->tokens->args_list->entry, next);
	if (get_args)
	{
		prev->next = get_args;
		free (tmp->content);
		tmp->content = NULL;
	}
	else
		prev->next = tmp;
	prev = (*env)->tokens->args_list;
	free_dir(env);	
}
*/

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
	while ((entry = readdir(current)))
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
	}
	closedir(current);
}

t_token	*new_word_token(char *str)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	new->type = TOKEN_WORD;
	new->value = ft_strdup(str);
	new->next = NULL;
	return (new);
}

int	expand_wildcard(char *input, t_env **env, t_token *prev, t_token *next)
{
	t_list		*get_args;
	t_list		*tmp;
	t_token		*new;
	if (!prev)
		return (0);
	get_current_dir(env);
	(*env)->envp_backup = (*env)->envp;
	get_args = expand_args(env, input);
	if (!get_args)
		return (0);
	while (get_args)
	{
		tmp = get_args;
		new = new_word_token(get_args->content);
		prev->next = new;
		prev = new;
		new->next = next;
		free (get_args->content);
		get_args = get_args->next;
		free (tmp);
	}
	free_dir(env);
	(*env)->envp = (*env)->envp_backup;
	// expanded = NULL;
	// expanded = strdup(buf.data);
	// buffer_free(&buf);
	// return (expanded);
	return (1);
}
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

int	glob(char *pattern, t_env *env, int i, int j)
{
	char	*file;

	file = env->directory_list->content;
	if (pattern[i] == 0 && file[j] == 0)
		return (1);
	if (pattern[i] == 0 || file[j] == 0)
		return (0);
	if (pattern[i] != '*' && pattern[i] != file[j])
		return (0);
	if (pattern[0] == '*' && file[0] == '.')
		return (0);
	if (pattern[i] == '*')
		return (glob(pattern, env, i + 1, j + 1)
			|| glob(pattern, env, i, j + 1)
			|| glob(pattern, env, i + 1, j));
	else
		return (glob(pattern, env, i + 1, j + 1));
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
		if (glob(pattern, *env, 0, 0))
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
	return (1);
}

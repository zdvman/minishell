/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 11:53:04 by dzuiev            #+#    #+#             */
/*   Updated: 2024/06/06 14:26:26 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	glob(char *pattern, t_env *env, int i, int j)
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

static void	update_matched(t_env **env, t_list **matched, t_list **matched_head)
{
	if (!*matched)
	{
		(*matched) = new_dir_entry((*env)->directory_list->content);
		(*matched_head) = (*matched);
	}
	else
	{
		(*matched)->next = new_dir_entry((*env)->directory_list->content);
		(*matched) = (*matched)->next;
	}
}

t_list	*expand_args(t_env **env, char *pattern, t_token *prev)
{
	t_list	*matched;
	t_list	*matched_head;

	if (!ft_strcmp("ls", prev->value))
		(*env)->ls = 1;
	else
		(*env)->ls = 0;
	matched = NULL;
	matched_head = NULL;
	(*env)->directory_list = (*env)->dir_head;
	while ((*env)->directory_list)
	{
		if (glob(pattern, *env, 0, 0))
			update_matched(env, &matched, &matched_head);
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
	get_args = expand_args(env, input, prev);
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

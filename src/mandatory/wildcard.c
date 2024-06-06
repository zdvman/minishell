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

static int	glob(char *pattern, char *file, int i, int j)
{
	if (pattern[i] == 0 && file[j] == 0)
		return (1);
	if (pattern[i] == 0 || file[j] == 0)
		return (0);
	if (pattern[i] != '*' && pattern[i] != file[j])
		return (0);
	if (pattern[0] == '*' && file[0] == '.')
		return (0);
	if (pattern[0] == '.' && pattern[1] == '*' && file[0] == '.')
		return (1);
	if (pattern[i] == '*')
		return (glob(pattern, file, i + 1, j + 1)
			|| glob(pattern, file, i + 1, j)
			|| glob(pattern, file, i, j + 1));
	else
		return (glob(pattern, file, i + 1, j + 1));
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
	int		i;

	i = 0;
	if (pattern[0] && pattern[0] == '*' && pattern[1])
		while (pattern[i + 1] && pattern[i + 1] == '*')
			i++;
	matched = NULL;
	matched_head = NULL;
	(*env)->directory_list = (*env)->dir_head;
	while ((*env)->directory_list)
	{
		if (glob(pattern, (*env)->directory_list->content, i, 0))
			update_matched(env, &matched, &matched_head);
		(*env)->directory_list = (*env)->directory_list->next;
		if (matched_head && !prev)
			break ;
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
	new->wildcard = 0;
	return (new);
}

t_token	*expand_wildcard(char *input, t_env **env, t_token *prev, t_token *next)
{
	t_list		*args;
	t_token		*new;
	t_token		*head;

	new = NULL;
	head = NULL;
	get_current_dir(env);
	args = expand_args(env, input, prev);
	if (args)
		args_to_tokens(args, next, &head);
	free_dir(env);
	return (head);
}

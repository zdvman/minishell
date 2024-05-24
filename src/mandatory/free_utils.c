/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 18:17:27 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/24 17:59:51 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	ft_free_tokens(t_token **tokens)
{
	t_token	*current;

	if (tokens == NULL || *tokens == NULL)
		return ;
	while (*tokens != NULL)
	{
		current = *tokens;
		*tokens = (*tokens)->next;
		if (current->value != NULL)
			free(current->value);
		free(current);
	}
	*tokens = NULL;
}

void	ft_free_ast(t_ast_node **node)
{
	int	i;

	if (!*node)
		return ;
	i = 0;
	if ((*node)->args)
	{
		while ((*node)->args[i])
		{
			if ((*node)->args[i])
			{
				free((*node)->args[i]);
				(*node)->args[i] = NULL;
			}
			i++;
		}
		free((*node)->args);
		(*node)->args = NULL;
	}
	ft_free_ast(&(*node)->left);
	ft_free_ast(&(*node)->right);
	free(*node);
	*node = NULL;
}

static void	ft_free_env(t_env **env)
{
	int	i;

	if (!*env)
		return ;
	i = 0;
	if ((*env)->envp)
	{
		while ((*env)->envp[i])
		{
			if ((*env)->envp[i])
			{
				free((*env)->envp[i]);
				(*env)->envp[i] = NULL;
			}
			i++;
		}
		if ((*env)->envp)
			free((*env)->envp);
		(*env)->envp = NULL;
	}
	free(*env);
	*env = NULL;
}

static void	ft_free_loc_vars(t_env **env)
{
	int	i;

	if (!*env)
		return ;
	i = 0;
	if ((*env)->loc_vars)
	{
		while ((*env)->loc_vars[i])
		{
			if ((*env)->loc_vars[i])
			{
				free((*env)->loc_vars[i]);
				(*env)->loc_vars[i] = NULL;
			}
			i++;
		}
		free((*env)->loc_vars);
		(*env)->loc_vars = NULL;
	}
}

void	ft_free_args(char ***args)
{
	int	i;

	if (args == NULL || *args == NULL)
		return ;
	i = 0;
	while ((*args)[i])
	{
		free((*args)[i]);
		(*args)[i] = NULL;
		i++;
	}
	free(*args);
	*args = NULL;
}

void	cleanup(t_env **env, int status)
{
	ft_free_tokens(&(*env)->tokens);
	if ((*env)->user_host)
		free ((*env)->user_host);
	if ((*env)->prompt)
		free ((*env)->prompt);
	ft_free_ast(&(*env)->ast);
	ft_free_loc_vars(env);
	ft_free_env(env);
	rl_clear_history();
	printf("\033[?12l");
	exit(status);
}

void	cleanup_loop(char **input, t_env **env)
{
	if (input && *input) 
		ft_free_str(input);
	ft_free_tokens(&(*env)->tokens);
	ft_free_ast(&(*env)->ast);
}

void	cleanup_no_exit(t_env **env)
{
	ft_free_tokens(&(*env)->tokens);
	ft_free_ast(&(*env)->ast);
}

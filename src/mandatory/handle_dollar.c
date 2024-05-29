/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_dollar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 08:24:02 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/29 11:05:41 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	insert_nl_t_v(char **start, char **input, t_dynamic_buffer *buf)
{
	if (*start != *input)
		buffer_append(buf, *start, *input - *start);
	(*input)++;
	if (**input == 'n')
		buffer_append_char(buf, '\n');
	else if (**input == 'v')
		buffer_append_char(buf, '\v');
	else if (**input == 't')
		buffer_append_char(buf, '\t');
	(*input)++;
	*start = *input;
}

static void	expand_cstyle_string(char **input, char **current,
				t_dynamic_buffer *buf)
{
	(*input)++;
	*current = *input;
	while (**input && **input != '\'')
	{
		if (**input == '\\'
			&& (*(*input + 1) == 'n'
				|| *(*input + 1) == 'v'
				|| *(*input + 1) == 't'))
			insert_nl_t_v(current, input, buf);
		else
			(*input)++;
	}
	if (*current != *input)
		buffer_append(buf, *current, *input - *current);
	if (**input == '\'')
		(*input)++;
	*current = *input;
}

static void	handle_environment_variable(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env)
{
	char	*env_name;
	char	*env_value;

	*current = *input;
	if (**input == '0')
	{
		buffer_append(buf, "minishell", 9);
		(*input)++;
	}
	else if (**input == '?')
	{
		if ((*env)->exit_status >= 0)
			env_value = ft_itoa((*env)->exit_status);
		else
			env_value = NULL;
		buffer_append(buf, env_value, ft_strlen(env_value));
		free(env_value);
		(*input)++;
	}
	else
	{
		while (**input && (ft_isalnum(**input) || **input == '_'))
			(*input)++;
		env_name = ft_substr(*current, 0, *input - *current);
		if (get_var(env_name, (*env)->envp))
			env_value = ft_strdup(get_var(env_name, (*env)->envp));
		else if (get_var(env_name, (*env)->loc_vars))
			env_value = ft_strdup(get_var(env_name, (*env)->loc_vars));
		else
			env_value = ft_strdup("");
		buffer_append(buf, env_value, ft_strlen(env_value));
		free(env_name);
		free(env_value);
	}
	if (env_value == NULL)
		buffer_append(buf, "", 0);
	*current = *input;
}

void	handle_dollar_sign(char **input, char **current, t_dynamic_buffer *buf,
			t_env **env)
{
	if (*current != *input)
		buffer_append(buf, *current, *input - *current);
	(*input)++;
	if (**input == '\'')
		expand_cstyle_string(input, current, buf);
	else if (ft_isalnum(**input) || **input == '_' || **input == '?')
		handle_environment_variable(input, current, buf, env);
	else
	{
		if (is_dollar_special_case(**input))
		{
			write(2, "$", 1);
			ft_putchar_fd(**input, 2);
			ft_putstr_fd(": not supported in this version of minishell\n", 2);
			(*env)->syntax_error = 1;
			return ;
		}
		buffer_append_char(buf, '$');
		buffer_append_char(buf, **input);
		(*input)++;
	}
	*current = *input;
}

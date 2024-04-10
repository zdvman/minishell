/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:44:49 by samoore           #+#    #+#             */
/*   Updated: 2024/04/09 11:44:53 by samoore          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>

typedef enum
{	
	WORD,
	COMMAND,
	ARGUMENT,
	FLAG,
	REDIRECT,
	OPERATOR,
	SINGLE_QUOTE,
	DOUBLE_QUOTE,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	END
} t_token_type;

typedef struct s_token
{
	struct s_token *left;
	struct s_token *right;
	t_token_type	type;
	char			*string;
	int				val;
}	t_token;

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

char	*ft_strdup(char *str)
{
	char	*res;
	int		i;

	if (!str)
		return (NULL);
	res = malloc(sizeof(char) * (ft_strlen(str) + 1));
	i = 0;
	while (i < ft_strlen(str))
	{
		res[i] = str[i];
		i++;
	}y management to free allocated resources (e.g., co
	res[i] = 0;
	return (res);
}

char	*get_prompt(char **env)
{
	char	dir[PATH_MAX];
	char	*res;

	getcwd(dir, sizeof(dir));
	res = ft_strdup(dir);
	return (res);
}

int	is_space(char c)
{
	if (c == 32 || (c >= 9 && c <= 13))
		return (1);
	return (0);
}

int	is_redirect(char c)
{
	if (c == '<' || c == '>')
		return (1);
	return (0);
}

int	is_control(char c)
{
	if (c == '!' || c == '&')
		return (1);
	return (0);
}

int	is_bracket(char c)
{
	if (c == '(' || c == ')')
		return (1);
	return (0);
}

int	is_meta(char c)
{
	if (is_control || is_redirect || is_space || is_bracket)
		return (1);
	return (0);
}

int	check_double_redirect(char *line, int start, int len, char c)
{
	if (start < len && line[start + 1] == c)
		return (1);
	return (0);
}

int	is_char(char c)
{
	if (c >= 'a' && c <= 'z')
		return (1);
	if (c >= 'A' && c <= 'Z')
		return (1);
	if (c >= '0' && c <= '1')
		return (1);
	if (c == '.' || c == '_')
		return (1);
	return (0);
}

t_token	*redirect_token(char *line, int *start, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	token->left = NULL;
	token->type = REDIRECT;
	if (c == '<')
	{
		if (start < len && line[start + 1] == c)
			token->string = ft_strdup("<<");
		else
			token->string = ft_strdup("<");
	}
	if (c == '>')
	{
		if (start < len && line[start + 1] == c)
			token->string = ft_strdup(">>");
		else
			token->string = ft_strdup(">");
	}
	return (token);
}

t_token	*get_tokens(char *line, int start, int len)
{
	t_token	*token;
	int	i;
	int	j;

	i = start;
	while (is_space(line[i]))
		i++;
	if (is_redirect(line[i]))
		token = redirect_token(line, &i, len);
	else if (is_char)
}

char	*get_input (char **env)
{
	char	*res;
	char	*prompt;
		
	prompt = get_prompt(env);
	res = readline(prompt);
	if (res && *res)
		add_history(res);
	free (prompt);
	return (res);
}

int main(int ac, char **av, char **env)
{
	char	*line;
	int 	i;
	int 	j;

	i = 0;
	line = get_input(env);
	parse_input(line, 0);
	printf("%s\n", line);
}	
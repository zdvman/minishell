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

#include "minishell.h"

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
	if (c == '|' || c == '&')
		return (1);
	return (0);
}

int	is_bracket(char c)
{
	if (c == '(')
		return (1);
	if (c == ')')
		return (2);
	return (0);
}


int	is_quote(char c)
{
	if (c == '\'' || c == '\"')
		return (1);
	return (0);
}

int	is_meta(char c)
{
	if (is_control(c) || is_redirect(c) || is_space(c) || is_bracket(c))
		return (1);
	if (is_quote(c))
		return (1);
	if (c == 0)
		return (1);
	return (0);
}

int	is_word(char c)
{
	if (c >= 'a' && c <= 'z')
		return (1);
	if (c >= 'A' && c <= 'Z')
		return (1);
	if (c >= '0' && c <= '9')
		return (1);
	if (c == '.' || c == '_')
		return (1);
	if (c == '/')
		return (1);
	if (c == '-')
		return (1);
	if (c == '~')
		return (1);
	if (c == '$')
		return (1);
	return (0);
}

t_token	*redirect_token(char *line, int *start, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	token->left = NULL;
	if (line[*start] == '<')
		token->type = REDIR_IN;
	if (line[*start] == '>')
		token->type = REDIR_OUT;
	if (*start < len - 1 && line[*start] == line[*start + 1])
	{
		token->type++;
		(*start)++;
	}
	(*start)++;
	return (token);
}

t_token	*end_token(void)
{
	t_token	*token;
	int		i;

	token = malloc(sizeof(t_token));
	token->type = END;
	token->right = NULL;
	return (token);
}

t_token	*bracket_token(char *line, int *start, int i)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	token->left = NULL;
	if (line[i] == '(')
		token->type = OPEN_BRACKET;
	else
		token->type = CLOSE_BRACKET;
	(*start)++;
	return (token);
}

t_token	*word_token(char *line, int *start, int len)
{
	t_token	*token;
	int		i;

	token = malloc(sizeof(t_token));
	token->type = WORD;
	token->left = NULL;
	i = *start;
	i++;
	while (i < len - 1 && !is_meta(line[i]))
		i++;
	if (is_meta(line[i]))
		i--;
	token->string = malloc(sizeof(char) * (i - *start + 1));
	strncpy(token->string, &line[*start], (i - *start + 1));
	token->string[i - *start + 1] = 0;
	if (!strcmp(token->string, "exit"))
		token->type = EXIT;
	*start = i + 1;
	return (token);
}

t_token	*quote_word_token(char *line, int *start, int len)
{
	t_token	*token;
	int		i;

	token = malloc(sizeof(t_token));
	token->type = WORD;
	token->left = NULL;
	i = *start;
	i++;
	while (i < len - 1 && line[i] != line[*start])
		i++;
	if (i == len || line[i] != line[*start])
	{
		printf("error - unclosed quote %c\n", line[*start]);
	//	exit (1);
	}
	token->string = malloc(sizeof(char) * (i - *start + 1));
	strncpy(token->string, &line[*start], (i - *start + 1));
	token->string[i - *start + 1] = 0;
	*start = i + 1;
	return (token);
}

t_token	*control_token(char *line, int *start, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	token->left = NULL;
	if (line[*start] == '|')
	{
		if (*start < (len - 1) && line[*start + 1] == '|')
			token->type = CONTROL;
		else
			token->type = PIPE;
	}
	//else if (*start == len - 1 || line[*start + 1] != '&')
	//	exit (2);    // need to add exit error function
	else
		token->type = CONTROL;
	if (token->type == CONTROL)
		(*start)++;
	(*start)++;
	return (token);
}

t_token	*get_tokens(char *line, int start, int len)
{
	t_token	*token;
	int	i;
	int	j;

	if (start == len)
		return (end_token ());
	i = start;
	while (is_space(line[i]))
		i++;
	if (!line[i])
		return (end_token ());
	else if (is_redirect(line[i]))
		token = redirect_token(line, &i, len);
	else if (is_bracket(line[i]))
		token = bracket_token (line, &i, i);
	else if (is_quote(line[i]))
		token = quote_word_token (line, &i, len);
	else if (is_word(line[i]))
		token = word_token(line, &i, len);
	else if (is_control(line[i]))
		token = control_token(line, &i, len);
	else
	{
		printf("%c not recognised\n", line[i++]);
		return (get_tokens(line, i, len));
	}
	token->right = get_tokens(line, i, len);
	return (token);
}

void	print_tokens(t_token *tokens)
{
	while (tokens)
	{
		if (tokens->type == WORD)
			printf("WORD: %s\n", tokens->string);
		if (tokens->type == REDIR_IN)
			printf("REDIR_IN\n");
		if (tokens->type == REDIR_IN_DBL)
			printf("REDIR_IN_DBL\n");
		if (tokens->type == REDIR_OUT)
			printf("REDIR_OUT\n");
		if (tokens->type == REDIR_OUT_DBL)
			printf("REDIR_OUT_DBL\n");
		if (tokens->type == PIPE)
			printf("PIPE\n");
		if (tokens->type == CONTROL)
			printf("CONTROL\n");
		if (tokens->type == DONE)
			printf("DONE\n");
		if (tokens->type == EXIT)
			printf("EXIT\n");
		if (tokens->type == OPEN_BRACKET)
			printf("OPEN_BRACKET\n");
		if (tokens->type == CLOSE_BRACKET)
			printf("CLOSE_BRACKET\n");
		if (tokens->type == COMMAND)
		{
			printf("COMMAND\n");
			int i = 0;
			while (tokens->args[i])
			{
				printf("arg %d: %s\n", i, tokens->args[i]);
				i++;
			}
			printf("path %s\n", tokens->command_path);
		}
		if (tokens->type == END)
			printf("END\n");
		tokens = tokens->right;
	}
}

void	link_tokens_left(t_token *tokens)
{
	t_token	*prev;

	prev = tokens;
	tokens = tokens->right;
	while (tokens->type != END)
	{
		tokens->left = prev;
		prev = tokens;
		tokens = tokens->right;
	}
	tokens->left = prev;
	while (tokens->left)
		(tokens = tokens->left);
}

char	*get_input (t_env *env)
{
	char	*res;
		
	res = readline("> ");
	if (res && *res)
		add_history(res);
	env->tokens = get_tokens(res, 0, ft_strlen(res));
	env->token_head = env->tokens;
	link_tokens_left(env->tokens);
	return (res);
}

int main(int ac, char **av, char **environ)
{
	char	*line;
	int 	status;
	t_env	env;
	pid_t	pid;

	env.env = environ;
	env.paths = NULL;
	env.paths = ft_split(get_path (&env), ':');
	if (env.paths)
		add_slash(env.paths);
	line = get_input(&env);
	parse_tokens(&env);
	while (env.tokens->type != END && env.tokens->type != EXIT)
	{
		pid = fork ();
		if (!pid)
		{
			if (env.tokens->type == COMMAND)
				execve(env.tokens->command_path, env.tokens->args, environ);
			exit (0);
		}
		else
		{
			waitpid(pid, &status, 0);
			if (env.tokens->type == COMMAND)
			{
				env.tokens->type = DONE;
				env.tokens->val = status;
				env.tokens = env.tokens->right;
			}
		}
		if (env.tokens->type != END)
			env.tokens = env.tokens->right;
		else
			break ;
	}
	env.tokens = env.token_head;
	print_tokens(env.tokens);
	printf("%d\n", env.tokens->val);
}	

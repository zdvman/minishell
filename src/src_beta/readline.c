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

t_token	*redirect_token(char *line, int *start, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
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
	token->left = NULL;
	token->args = NULL;
	token->command_path = NULL;
	token->args = NULL;
	token->string = NULL;
	token->redirects = NULL;
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
	if (line[i] == '(')
		token->type = OPEN_BRACKET;
	else
		token->type = CLOSE_BRACKET;
		(*start)++;
		token->left = NULL;
	token->args = NULL;
	token->command_path = NULL;
	token->args = NULL;
	token->string = NULL;
	token->redirects = NULL;
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
	while (!is_meta(line[i]))
		i++;
	token->string = malloc(sizeof(char) * (i - *start + 1));
	strncpy(token->string, &line[*start], (i - *start));
	token->string[i - *start] = 0;
	if (!strcmp(token->string, "exit"))
		token->type = EXIT;
	*start = i;
		token->left = NULL;
	token->args = NULL;
	token->redirects_head = NULL;
	token->command_path = NULL;
	token->redirects = NULL;
	return (token);
}

t_token	*quote_word_token(char *line, int *start, int len)
{
	t_token	*token;
	int		i;

	token = malloc(sizeof(t_token));
	token->type = WORD;
	i = *start;
	i++;
	while (i < len && line[i] != line[*start])
		i++;
	if (line[i] != line[*start])
	{
		printf("error - unclosed quote %c   %c\n", line[*start], line[i]);
	//	exit (1);
	}
	token->string = malloc(sizeof(char) * (i - *start + 2));
	strncpy(token->string, &line[*start], (i - *start + 1));
	token->string[i - *start + 1] = 0;
	*start = i + 1;
	token->left = NULL;
	token->args = NULL;
	token->command_path = NULL;
	token->args = NULL;
	token->redirects = NULL;
	return (token);
}

t_token	*control_token(char *line, int *start, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (line[*start] == '|')
	{
		if (*start < len - 1 && line[*start + 1] == '|')
			token->type = CONTROL;
		else
			token->type = PIPE;
		token->control = '|';
	}
	//else if (*start == len - 1 || line[*start + 1] != '&')
	//	exit (2);    // need to add exit error function
	else
	{
		token->type = CONTROL;
		token->control = '&';
	}
	if (token->type == CONTROL)
		(*start)++;
	(*start)++;
	token->left = NULL;
	token->string = NULL;
	token->command_path = NULL;
	token->args = NULL;
	token->redirects = NULL;
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
	while (line[i] && is_space(line[i]))
		i++;
	if (i >= len)
		return (get_tokens(line, len, len));
	if (is_redirect(line[i]))
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
		//printf("%c not recognised\n", line[i++]);
		return (get_tokens(line, len, len));
	}
	token->args = NULL;
	token->command_path = NULL;
	token->left = NULL;
	token->right = get_tokens(line, i, len);
	return (token);
}

void	link_tokens_left(t_env *env)
{
	t_token	*prev;

	prev = env->tokens;
	env->tokens = env->tokens->right;
	while (env->tokens->type != END)
	{
		env->tokens->left = prev;
		prev = env->tokens;
		env->tokens = env->tokens->right;
	}
	env->tokens->left = prev;
	while (env->tokens->left)
		(env->tokens = env->tokens->left);
}

char	*get_input (t_env *env)
{
	char	*res;
		
	res = readline("\n> ");
	if (res && *res)
	{
		add_history(res);
		env->tokens = get_tokens(res, 0, ft_strlen(res));
		env->token_head = env->tokens;
	}
	else
	{
		if (res)
			free (res);
		return (NULL);
	}
	return (res);
}

char	*build_line_from_av(int ac, char **av, t_env *env)
{
	char	*tmp;
	char	*res;
	int		i;

	res = ft_calloc(1, 1);
	i = 1;
	while (i < ac)
	{
		tmp = ft_strjoin(res, av[i]);
		free (res);
		res = ft_strjoin(tmp, " ");
		free (tmp);
		i++;
	}
	env->tokens = get_tokens(res, 0, ft_strlen(res));
	env->token_head = env->tokens;
	link_tokens_left(env);
	return (res);
}

void	pipe_child(t_env *env, int fd[2])
{
	t_redirect *tmp;
	int			file;

	close (fd[0]);
	dup2 (fd[1], STDOUT_FILENO);
	close (fd[1]);
	if (env->tokens->redirects_head)
	{
		tmp = env->tokens->redirects_head;
		while (tmp)
		{
			if (tmp->type == REDIR_IN)
			{
				file = open(tmp->file, O_RDONLY);
				dup2(file, STDIN_FILENO);
			}
			if (tmp->type == REDIR_OUT)
			{
				file = open(tmp->file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
				dup2(file, STDOUT_FILENO);
			}
			tmp = tmp->next;
		}
	}
	execve (env->tokens->command_path, env->tokens->args, env->env);
}

void	single_child(t_env *env, int fd[2])
{
	t_redirect *tmp;
	int			file;

	if (env->tokens->redirects_head)
	{
		tmp = env->tokens->redirects_head;
		while (tmp)
		{
			if (tmp->type == REDIR_IN)
			{
				file = open(tmp->file, O_RDONLY);
				dup2(file, STDIN_FILENO);
			}
			if (tmp->type == REDIR_OUT)
			{
				file = open(tmp->file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
				dup2(file, STDOUT_FILENO);
			}
			tmp = tmp->next;
		}
	}
	execve (env->tokens->command_path, env->tokens->args, env->env);
}

void	pipe_parent(int fd[2])
{
	close (fd[1]);
	dup2 (fd[0], STDIN_FILENO);
	close (fd[0]);
}

void	single_parent(int fd[2])
{
	int	file;

	file = open(".temp_file", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	
	close (fd[1]);
	dup2 (STDOUT_FILENO, STDIN_FILENO);
	close (fd[0]);
}

void	evaluate(t_env *env)
{
	pid_t	pid;
	int		fd[2];
	int		status;

	while (env->tokens->type != END)
	{
		if (env->tokens->type == COMMAND)
		{
			if (env->tokens->pipe == 1)
			{
				while (env->tokens->pipe == 1)
				{
					pipe(fd);
					pid = fork ();
					if (!pid)
						pipe_child(env, fd);
					else
						pipe_parent(fd);
					env->tokens = env->tokens->right;
				}
			}
			else
			{
				pid = fork ();
				if (!pid)
					single_child(env, fd);
				else
				{
					if (env->tokens->left && env->tokens->left->pipe)
						single_parent(fd);
					waitpid(pid, &status, 0);
					env->tokens->type = DONE;
					env->tokens->val = status;
				}
				env->tokens = env->tokens->right;
			}
		}
		if (status == 0 && env->tokens->type == CONTROL)
		{
			if (env->tokens->control == '&')
				env->tokens = env->tokens->right;
			else
				env->tokens = env->tokens->right->right;
		}
		if (status != 0 && env->tokens->type == CONTROL)
		{
			if (env->tokens->control == '&')
				env->tokens = env->tokens->right->right;
			else
				env->tokens = env->tokens->right;
		}
	}
	env->tokens = env->token_head;
}

void	shell_loop(t_env *env, int ac, char **av)
{
	char	*line;

	line = NULL;
	if (ac > 1)
		line = build_line_from_av(ac, av, env);
	while (1)
	{
		if (ac == 1)
		while (!line || !*line)
			line = get_input(env);
		if (env->tokens->type == EXIT)
			break ;
		parse_tokens(env);
		evaluate(env);
		env->tokens = env->token_head;
		//print_tokens(env->tokens);
		free (line);
		line = NULL;
		reset_tokens(env);
		env->tokens = NULL;
		if (ac > 1)
			break ;
	}
	free (line);
}

int main(int ac, char **av, char **environ)
{
	t_env	env;

	env.tokens = NULL;
	env.paths = NULL;
	env.env = environ;
	env.paths = ft_split(get_path (&env), ':');
	if (env.paths)
		add_slash(env.paths);
	using_history ();
	shell_loop(&env, ac, av);
	clear_env(&env);
	return (1);
}	

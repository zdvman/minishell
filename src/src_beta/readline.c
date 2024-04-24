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

	token = ft_calloc(sizeof(t_token), 1);
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

	token = ft_calloc(sizeof(t_token), 1);
	token->type = END;
	return (token);
}

t_token	*bracket_token(char *line, int *start, int i)
{
	t_token	*token;

	token = ft_calloc(sizeof(t_token), 1);
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

	token = ft_calloc(sizeof(t_token), 1);
	token->type = WORD;
	i = *start;
	while (!is_meta(line[i]))
		i++;
	token->string = ft_calloc(sizeof(char) * (i - *start + 1), 1);
	strncpy(token->string, &line[*start], (i - *start));
	token->string[i - *start] = 0;
	if (!strcmp(token->string, "exit"))
		token->type = EXIT;
	*start = i;
	return (token);
}

t_token	*quote_word_token(char *line, int *start, int len)
{
	t_token	*token;
	int		i;

	token = ft_calloc(sizeof(t_token), 1);
	token->type = WORD;
	i = *start;
	i++;
	while (i < len && line[i] != line[*start])
		i++;
	// if (line[i] != line[*start])
	// {
	// 	printf("error - unclosed quote %c   %c\n", line[*start], line[i]);
	// 	return (NULL);
	// }
	token->string = ft_calloc(sizeof(char) * (i - *start + 2), 1);
	strncpy(token->string, &line[*start], (i - *start + 1));
	token->string[i - *start + 1] = 0;
	*start = i + 1;
	return (token);
}

t_token	*control_token(char *line, int *start, int len)
{
	t_token	*token;

	token = ft_calloc(sizeof(t_token), 1);
	if (line[*start] == '|')
	{
		if (*start < len - 1 && line[*start + 1] == '|')
			token->type = CONTROL;
		else
			token->type = PIPE;
		token->control = '|';
	}
	else
	{
		token->type = CONTROL;
		token->control = '&';
	}
	if (token->type == CONTROL)
		(*start)++;
	(*start)++;
	return (token);
}

t_token	*get_tokens(char *line, int i, int len)
{
	t_token	*token;

	if (i == len)
		return (end_token ());
	while (line[i] && is_space(line[i]))
		i++;
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
		return (end_token ());                  /// what do we do when unrecognised token
	token->right = get_tokens(line, i, len);
	return (token);
}

void	link_tokens_left(t_env *env)
{
	t_token	*prev;

	env->tokens = env->token_head;
	prev = env->tokens;
	env->tokens = env->tokens->right;
	while (env->tokens->type != END)
	{
		env->tokens->left = prev;
		prev = env->tokens;
		env->tokens = env->tokens->right;
	}
	env->tokens->left = prev;
	env->tokens = env->token_head;
}

char	*get_input (t_env *env)
{
	char	*res;
		
	rl_on_new_line ();
	res = readline("> ");
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

void	open_redirect_file(t_redirect *tmp)
{
	int	file;

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
			open_redirect_file(tmp);
			tmp = tmp->next;
		}
	}
	execve (env->tokens->command_path, env->tokens->args, env->env);
}

void	single_child(t_env *env)
{
	t_redirect *tmp;
	int			file;

	if (env->tokens->redirects_head)
	{
		tmp = env->tokens->redirects_head;
		while (tmp)
		{
			open_redirect_file(tmp);
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

void	pipe_command(t_env *env)
{
	pid_t	pid;
	int		fd[2];
	int		res;

	while (env->tokens->pipe == 1)
	{
		pipe(fd);
		pid = fork ();
		if (!pid)
			pipe_child(env, fd);
		else
		{
			pipe_parent(fd);
			// waitpid(pid, &res, WNOHANG);
		}
		env->tokens = env->tokens->right;
	}
}

int	 single_command(t_env *env)
{
	pid_t	pid;
	int		fd[2];
	int		return_val;

	pid = fork ();
	if (!pid)
		single_child(env);
	else
	{
		dup2 (STDOUT_FILENO, STDIN_FILENO);
		waitpid(pid, &return_val, 0);
	}
	env->tokens = env->tokens->right;
	return (return_val);
}


void	get_next_token(t_env *env)
{
	int	brackets;

	brackets = 1;
	while (brackets && env->tokens->type != END)
	{
		env->tokens = env->tokens->right;
		if (env->tokens->type == OPEN_BRACKET)
			brackets++;
		if (env->tokens->type == CLOSE_BRACKET)
			brackets--;
	}
}

void	check_control(t_env *env, int return_val)
{
	if (env->tokens->type == CONTROL)
	{
		if (return_val == 0)
		{
			if (env->tokens->control == '&')
				env->tokens = env->tokens->right;
			else
			{
				env->tokens = env->tokens->right;
				if (env->tokens->type == OPEN_BRACKET)
					get_next_token(env);
				else
					env->tokens = env->tokens->right;
			}
		}
		if (return_val != 0)
		{
			if (env->tokens->control == '|')
				env->tokens = env->tokens->right;
			else
			{
				env->tokens = env->tokens->right;
				if (env->tokens->type == OPEN_BRACKET)
					get_next_token(env);
				else
					env->tokens = env->tokens->right;
			}
		}
	}
}

int	evaluate(t_env *env)
{
	int	return_val;
	int	i;


	while (env->tokens->type != END && env->tokens->type != CLOSE_BRACKET)
	{
		i = 1;
		check_control(env, return_val);
		if (env->tokens->type == OPEN_BRACKET)
		{
			env->tokens = env->tokens->right;
			return_val = evaluate(env);
		}
		else if (env->tokens->type == COMMAND)
		{
			if (add_path(env))
			{
				while (env->tokens->type != END)
					env->tokens = env->tokens->right;
				return (1);
			}
			i++;
			if (env->tokens->pipe == 1)
				pipe_command(env);
			else
				return_val = single_command(env);
		}
	}
	while (waitpid(0, NULL, 0) != -1);

	env->exit_status = return_val;
	env->tokens = env->tokens->right;
	return (return_val);
}

int	check_grammar(t_env *env)
{
	while (env->tokens)
	{
		if (env->tokens->type == CONTROL)
		{
			if (!env->tokens->left && env->tokens->left->type != CLOSE_BRACKET && env->tokens->left->type != COMMAND)
			{
				printf("unexpected token \'%c\'  %d\n", env->tokens->control, env->tokens->left->type);
				return (1);
			}
			if (env->tokens->right->type != COMMAND && env->tokens->right->type != OPEN_BRACKET)
			{
				printf("::unexpected token \'%c\'\n", env->tokens->control);
				return (1);
			}
			
		}
		env->tokens = env->tokens->right;
	}
	env->tokens = env->token_head;
	return (0);
}

t_list	*new_dir_entry(char *entry_name)
{
	t_list	*new;

	new = ft_calloc(sizeof(t_list), 1);
	new->entry = ft_strdup(entry_name);
	return (new);
}

int	file_has_less_dots(char *s1, char *s2)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (*s1)
		if (*s1++ == '.')
			i++;
	while (*s2)
		if (*s2++ == '.')
			j++;
	if (j < i)
		return (1);
	return (0);
}

int	glob(char *pattern, char *file_name, int i, int j)
{
	int	path1;
	int	path2;
	int	path3;

	if (pattern[i] == 0 && file_name[j] == 0)
		return (1);
	if (file_name[j] == 0 && pattern[i] == '*')
		return (glob(pattern, file_name, i + 1, j));
	if (pattern[i] == 0 || file_name[j] == 0)
		return (0);

	if (pattern[i] != '*' && pattern[i] != file_name[j])
		return (0);
	if (pattern[0] == '*' && file_name[0] == '.')
		return(0);
	if (pattern[0] == '*' && pattern[1] == '.' && file_name[0] == '.')
		return(0);
	if (pattern[0] == '.' && pattern[1] == '*' && !pattern[2] && file_name[0] == '.')
		return(1);
	path1 = 0;
	path2 = 0;
	path3 = 0;
	if (pattern[i] == '*')
	{
		path1 = glob(pattern, file_name, i + 1, j + 1);
		path2 = glob(pattern, file_name, i, j + 1);
		path3 = glob(pattern, file_name, i + 1, j);
		return (path1 || path2 || path3);
	}
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

int	count_list(t_list *list)
{
	int	i;

	i = 1;
	if (!list)
		return (0);
	while (list)
	{
		i++;
		list = list->next;
	}
	return (i);
}

void	get_current_dir(t_env *env)
{
	struct dirent	*entry;
	char			*dir_name;
	DIR				*current;

	dir_name = malloc(256);
	dir_name = getcwd(dir_name, 256);
	current = opendir(dir_name);
	free (dir_name);
	while ((entry = readdir(current)))
	{
		if (!env->dir_head)
		{
			env->directory_list = new_dir_entry(entry->d_name);
			env->dir_head = env->directory_list;
		}
		else
		{
			env->directory_list->next = new_dir_entry(entry->d_name);
			env->directory_list = env->directory_list->next;
		}
	}
	closedir(current);
}

t_list	*expand_args(t_env *env, char *pattern, t_list *next)
{
	t_list	*prev;
	t_list	*matched;
	t_list	*matched_head;

	matched = NULL;
	matched_head = NULL;
	env->directory_list = env->dir_head;
	while (env->directory_list)
	{
		if (glob(pattern, env->directory_list->entry, 0, 0))
		{
			if (!matched)
			{
				matched = new_dir_entry(env->directory_list->entry);
				matched_head = matched;
			}
			else
			{
				matched->next = new_dir_entry(env->directory_list->entry);
				matched = matched->next;
			}
		}
		env->directory_list = env->directory_list->next;
	}
	if (matched)
		matched->next = next;
	return (matched_head);
}

void	free_dir(t_env *env)
{
	t_list	*tmp;

	env->directory_list = env->dir_head;
	while (env->directory_list)
	{
		tmp = env->directory_list;
		free (env->directory_list->entry);
		env->directory_list = env->directory_list->next;
		free (tmp);
	}
	env->directory_list = NULL;
	env->dir_head = NULL;
}

void	process_entries(t_env *env, t_list *prev)
{
	t_list	*next;
	t_list	*tmp;
	t_list	*get_args;

	get_current_dir(env);
	if (contains(env->tokens->args_list->entry, '*'))
	{
		next = env->tokens->args_list->next;
		tmp = env->tokens->args_list;
		get_args = expand_args(env, env->tokens->args_list->entry, next);
		if (get_args)
		{
			prev->next = get_args;
			free (tmp->entry);
			tmp->entry = NULL;
		}
		else
			prev->next = tmp;
	}
	prev = env->tokens->args_list;
	free_dir(env);	
}

void	expand_wildcards(t_env *env)
{
	t_list	*prev;

	while (env->tokens->type != END)
	{
		if (env->tokens->type == COMMAND)
		{
			prev = env->tokens->args_list;
			env->tokens->args_list = env->tokens->args_head;
			// env->tokens->args_list = env->tokens->args_list->next;
			while ((env->tokens->args_list = env->tokens->args_list->next))
				process_entries(env, prev);
			env->tokens->args_list = env->tokens->args_head;
		}
		env->tokens = env->tokens->right;
	}
	env->tokens = env->token_head;
}

void	make_args_arrays(t_env *env)
{
	t_list	*tmp;
	int		i;

	env->tokens = env->token_head;
	env->tokens->args_list = env->tokens->args_head;
	while (env->tokens)
	{
		i = 0;
		if (env->tokens->args_list)
			env->tokens->args = ft_calloc(sizeof(char *) * count_list(env->tokens->args_list), 1);
		while (env->tokens->args_list)
		{
			env->tokens->args[i++] = ft_strdup(env->tokens->args_list->entry);
			tmp = env->tokens->args_list;
			free (tmp->entry);
			env->tokens->args_list = env->tokens->args_list->next;
			free (tmp);
		}
		env->tokens = env->tokens->right;
	}
}

int	check_commands_not_directories(t_env *env)
{
	struct stat	file;

	env->tokens = env->token_head;
	while (env->tokens)
	{
		if (env->tokens->type == COMMAND)
		{
			if (!(stat(env->tokens->args_list->entry, &file)))
				if ((file.st_mode & S_IFMT) == S_IFDIR)
				{
					if (contains(env->tokens->args_list->entry, '/'))
					{
						printf("bash: %s: Is a directory\n", env->tokens->args_list->entry);
						env->exit_status = 126;
					}
					else
						printf("%s: command not found\n", env->tokens->args_list->entry);
					return (1);
				}
		}
		env->tokens = env->tokens->right;
	}
	env->tokens = env->token_head;
	return (0);
}

int	parse_and_expand(t_env *env)
{
	if ((parse_tokens(env)) == 1)
		return (1);
	check_pipes(env);
	env->tokens = env->token_head;
	link_tokens_left(env);
	if ((check_grammar(env)) == 1)
		return (1);
	expand_wildcards(env);
	if ((check_commands_not_directories(env)) == 1)
		return (1);
	make_args_arrays(env);
	env->tokens = env->token_head;
	return (0);
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
		if (env->tokens->type != END && !(parse_and_expand(env)))
			evaluate(env);
		free (line);
		line = NULL;
		env->tokens = env->token_head;
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
	env.dir_head = NULL;
	env.env = environ;
	env.paths = ft_split(get_path (&env), ':');
	if (env.paths)
		add_slash(env.paths);
	using_history ();
	shell_loop(&env, ac, av);
	clear_env(&env);
	return (1);
}	

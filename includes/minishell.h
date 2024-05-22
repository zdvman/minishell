/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 10:29:58 by dzuiev            #+#    #+#             */
/*   Updated: 2024/05/20 16:22:41 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <stdbool.h>
# include <stdlib.h>
# include <signal.h>
# include <string.h>
# include <pthread.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"
# include <dirent.h>	

/* ************************************************************************** */
/*                                                                            */
/*  typedef enum e_token_type {                                               */
/*        TOKEN_WORD,            word                                         */
/*        TOKEN_REDIR_INPUT,     <                                            */
/*        TOKEN_REDIR_OUTPUT,    >                                            */
/*        TOKEN_REDIR_APPEND,    >>                                           */
/*        TOKEN_HERE_DOC,        <<                                           */
/*        TOKEN_OPEN_BRACKET,    (                                            */
/*        TOKEN_CLOSE_BRACKET,   )                                            */
/*        TOKEN_PIPE,            |                                            */
/*		  TOKEN_BACKGROUND,      &                                            */
/*        TOKEN_AND_IF,          &&                                           */
/*        TOKEN_OR_IF,           ||                                           */
/*        TOKEN_SEMI,            ;                                            */
/*        TOKEN_EOF              end of file - for lexer termination condition*/
/*    } token_type;                                                           */
/*                                                                            */
/* ************************************************************************** */

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_REDIR_INPUT,
	TOKEN_REDIR_OUTPUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HERE_DOC,
	TOKEN_PIPE,
	TOKEN_AND_IF,
	TOKEN_OR_IF,
	TOKEN_SEMI,
	TOKEN_BACKGROUND,
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_EOF
}				t_token_type;

typedef struct s_ast_node
{
	t_token_type 		type;          // Тип узла (команда, пайплайн, логический оператор)
	char				**args;        // Аргументы команды
	struct s_ast_node	*left;         // Левый ребенок в дереве (для операторов)
	struct s_ast_node	*right;        // Правый ребенок в дереве (для операторов)
}				t_ast_node;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				has_space;
	struct s_token	*next;
	struct s_token	*prev;
}				t_token;

typedef struct s_env
{
	char			**envp;
	char			**envp_backup;
	int				exit_status;
	int				pipe_fd[2];
	int				fd_in;
	int				fd_out;
	t_ast_node		*ast;
	t_token			*tokens;
	t_token			*head_token;
	t_list			*directory_list;
	t_list			*dir_head;
	char			**local_variables;
}				t_env;

typedef struct s_dynamic_buffer
{
	char	*data;
	size_t	len;
	size_t	capacity;
}				t_dynamic_buffer;

// init.c
void	shell_init(t_env **env, char **envp);

// dynamic_buffer.c
void	buffer_init(t_dynamic_buffer *buf);
void	buffer_free(t_dynamic_buffer *buf);
void	buffer_clear(t_dynamic_buffer *buf);
int		buffer_append_char(t_dynamic_buffer *buf, char c);
int		buffer_append(t_dynamic_buffer *buf, const char *str, size_t n);

// free_utils.c
void	ft_free_args(char ***args);
void	cleanup_loop(char **input, t_env **env);
void	cleanup(t_env **env, int status);
void	ft_free_ast(t_ast_node **node);

// utils.c
void	set_sig_actions(void);

// expand.c
void	expand_tokens(t_env **env);
char	*expand_word(t_env **env, char **input);

// multiline_and_quotes_input.c
char	*read_multiline(void);
int		is_quote_open(const char *input);

// handle_meta_redirs_pipe_or.c
void	handle_greater_than_sign(t_env **env, char **input);
void	handle_less_than_sign(t_env **env, char **input);
void	handle_pipe_or(t_env **env, char **input);
void	handle_and(t_env **env, char **input);

// handle_meta_brackets_semi.c
void	handle_semicolon(t_env **env, char **input);
void	handle_open_bracket(t_env **env, char **input);
void	handle_close_bracket(t_env **env, char **input);

// handle_quotes.c
void	handle_quotes(char **input, char **current, t_dynamic_buffer *buf, t_env **env);

// handle_dollar.c
void	handle_dollar_sign(char **input, char **current, t_dynamic_buffer *buf, t_env **env);

// get_tokens.c
void	add_token(t_token_type type, char *value, int space_after, t_env **env);
int		is_meta_character(char c);
void	handle_meta(t_env **env, char **input);
void	get_tokens(char *input, t_env **env);

extern volatile sig_atomic_t g_sigint_received;

// ast.c
t_ast_node	*new_ast_node(t_token_type type, char **args,
				t_ast_node *left, t_ast_node *right);
char		**copy_args(t_token **current, t_env **env);
t_ast_node	*parse_tokens(t_env **env);
// t_ast_node	*parse_expression(t_token **current, t_env **env);
t_ast_node	*parse_expression(t_token **current, t_env **env);
t_ast_node	*parse_term(t_token **current, t_env **env);
t_ast_node	*parse_redirection(t_token **current, t_env **env);

// parsing.c
bool		is_redirection(t_token_type type);
bool		is_control_op(t_token_type type);
t_ast_node	*parse_command(t_token **current, t_env **env);
t_ast_node	*parse_pipeline(t_token **current, t_env **env);
t_ast_node	*parse_sequence(t_token **current, t_env **env);
t_ast_node	*parse_bracket(t_token **current, t_env **env);

// get_path.c
char	*get_path(char *cmd, t_env **env);

// execute.c
void	handle_fd(t_env **env);
void	execute(t_ast_node *node, t_env **env);
void	recursive_execute(t_ast_node *node, t_env **env);

//main.c
void	print_token_name(t_token *token);

//wildcard.c  for bonus but using here for testing
int		contains(char *str, char target);
int		expand_wildcard(char *input, t_env **env, t_token *prev, t_token *next);

//builtin.c
void	execute_builtin(t_env **env, char **args);
void	insert_local(t_env *env, char *var);
int		is_assignment(char *cmd);
int		is_builtin(char *cmd);
void	remove_var(t_env *env, int target);

void	add_local_var(t_env **env, char *var);
void	add_var(t_env *env, char *var, char *val);
void	assign_variable(t_env *env, char *string);
char	*make_var(char *var, char *val);
int	export_var(t_env *env, char **args);
char	*get_env_variable(t_env *env, char *env_var);
char	*get_local_variable(t_env *env, char *env_var);
int	replace_or_add_env_var(t_env **env, char *var, char *val);

#endif

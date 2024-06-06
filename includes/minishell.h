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
# include <sys/ioctl.h>
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

# define MAX_CHILDREN 1024

extern volatile sig_atomic_t	g_signal;

typedef enum s_cmd
{
	ADD,
	REMOVE,
	GET_NUM,
	KILL
}	t_cmd;

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
	t_token_type		type;	
	// Тип узла (команда, пайплайн, логический оператор)
	char				**args;	
	// Аргументы команды
	struct s_ast_node	*left;	
	// Левый ребенок в дереве (для операторов)
	struct s_ast_node	*right;	
	// Правый ребенок в дереве (для операторов)
}				t_ast_node;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				has_space;
	int				wildcard;
	struct s_token	*next;
	struct s_token	*prev;
}				t_token;

typedef struct s_env
{
	int				syntax_error;
	char			**envp;
	int				exit_status;
	int				pipe_fd[2];
	int				fd_in;
	int				fd_out;
	t_ast_node		*ast;
	t_token			*tokens;
	t_token			*head_token;
	t_list			*directory_list;
	t_list			*dir_head;
	char			**loc_vars;
	char			*user_host;
	char			*prompt;
	char			**input;
	int				stdout;
	int				stdin;
	int				stderr;
}				t_env;

typedef struct s_dynamic_buffer
{
	char	*data;
	size_t	len;
	size_t	capacity;
}				t_dynamic_buffer;

//void	print_token_name(t_token *token)
void		print_token_name(t_token *token);
void		print_escaped(FILE *stream, const char *str);
void		print_ast_dot(t_ast_node *node, FILE *stream);
void		print_tokens(t_token *current);

// init.c
void		shell_init(t_env **env, char **envp);

// dynamic_buffer.c
void		buffer_init(t_dynamic_buffer *buf);
void		buffer_free(t_dynamic_buffer *buf);
void		buffer_clear(t_dynamic_buffer *buf);
int			buffer_append_char(t_dynamic_buffer *buf, char c);
int			buffer_append(t_dynamic_buffer *buf, const char *str, size_t n);

// free_utils_1.c
void		ft_free_tokens(t_token **tokens);
void		ft_free_env(t_env **env);
void		ft_free_ast(t_ast_node **node);
void		cleanup(t_env **env, int status);

// free_utils_2.c
void		cleanup_loop(char **input, t_env **env);
void		cleanup_no_exit(t_env **env);
void		ft_free_args(char ***args);

// utils.c
void		set_sig_actions(void);
void		exit_minishell(t_env **env);
int			pid_list(t_cmd function, pid_t pid);

// expand.c
void		expand_tokens(t_env **env, t_token *token, t_token *prev);
char		*expand_word(t_env **env, char **input);

// expand_utils.c
int			star_check_in_token(char *token_value);

// multiline_input.c
char		*read_multiline(t_env **env);

// multiline_input_utils.c
int			is_quote_open(const char *input);
int			pipe_or_and_is_closed(char *buffer_str, int i);

// handle_meta_redirs_pipe_or.c
void		handle_greater_than_sign(t_env **env, char **input);
void		handle_less_than_sign(t_env **env, char **input);
void		handle_pipe_or(t_env **env, char **input);
void		handle_and(t_env **env, char **input);

// handle_meta_brackets_semi.c
void		handle_semicolon(t_env **env, char **input);
void		handle_open_bracket(t_env **env, char **input);
void		handle_close_bracket(t_env **env, char **input);

// handle_quotes.c
void		handle_quotes(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env);
void		handle_double_quotes(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env);
void		handle_single_quotes(char **input, char **current,
				t_dynamic_buffer *buf);
void		handle_backslach(char **input, char **current,
				t_dynamic_buffer *buf);

// handle_dollar.c
void		handle_dollar_sign(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env);
void		handle_dollar_sign_in_quotes(char **input, char **current,
				t_dynamic_buffer *buf, t_env **env);
void		handle_backslach_out_of_quotes(char **input, char **current,
				t_dynamic_buffer *buf);

// get_tokens.c
void		add_token(t_token_type type, char *value,
				int space_after, t_env **env);
int			is_meta_character(char c);
void		handle_meta(t_env **env, char **input);
void		get_tokens(char *input, t_env **env);

// get_tokens_utils.c
int			handle_dollar_backtick_exception(char **input, char **current,
				t_env **env);
void		handle_dollar_special_case(char *input, t_env **env);
void		handle_backtick(char **input, t_env **env);
void		insert_nl_t_v(char **start, char **input, t_dynamic_buffer *buf);

// ast_builder_1.c
t_ast_node	*create_ast_node(t_token_type type, char **args, t_env **env);
char		**copy_args(t_token **current, t_env **env);
t_ast_node	*parse_tokens(t_env **env);

// ast_builder_2.c
t_ast_node	*parse_brackets(t_token **current, t_env **env);
t_ast_node	*parse_command(t_token **current, t_env **env);
t_ast_node	*parse_term(t_token **current, t_env **env);
t_ast_node	*parse_expression(t_token **current, t_env **env);

// ast_builder_3.c
t_ast_node	*parse_redirection(t_token **current, t_env **env);

// ast_builder_utils_1.c
t_token		**next_token(t_token **current);
t_ast_node	*ft_last_node(t_ast_node *base_node);
t_ast_node	*tree_grafter(t_ast_node *base_node, t_ast_node *new_node);
t_ast_node	*reroot_tree(t_ast_node *base_node, t_ast_node *new_node);
t_ast_node	*append_tree(t_ast_node *base_node, t_ast_node *new_node);

// ast_builer_utils_2.c
bool		is_redirection(t_token_type type);
bool		is_control_op(t_token_type type);

// get_path.c
char		*get_path(char *cmd, t_env **env);
char		**path_helper(t_env **env);

// execute.c
void		execute_semi(t_ast_node *node, t_env **env);
void		execute_and(t_ast_node *node, t_env **env);
void		execute_or(t_ast_node *node, t_env **env);
void		execute_background(t_ast_node *node, t_env **env);
void		execute(t_ast_node *ast, t_env **env);

//  execute_pipes_commands.c
void		execute_command(t_ast_node *node, t_env **env);
void		execute_pipe(t_ast_node *node, t_env **env);

// execute_redirections.c
void		execute_redir_output(t_ast_node *node, t_env **env);
void		execute_redir_append(t_ast_node *node, t_env **env);
void		execute_redir_input(t_ast_node *node, t_env **env);

// execute_here_doc.
void		free_list(t_list **list);
int			check_for_wildcard(t_env **env, char **input);
void		execute_here_doc(t_ast_node *node, t_env **env);

// execute_utils_1.c
void		wait_for_process(pid_t pid, t_env **env);
void		if_error(bool status, t_env **env);
void		set_origin_fd(int *origin_fd);
void		restore_origin_fd(int *origin_fd, t_env **env);
int			here_doc_signal_handler(t_env **env, int fd, int *origin_fd);

// execute_utils_2.c
void		pipe_fd_handler(int *fd, t_env **env, pid_t pid);
int			cmd_is_not_valid(char *cmd, t_env **env);
void		handle_fd(t_env **env);

int			is_a_directory(char *cmd, t_env *env);
int			is_exec(char *name);

//main.c
void		print_token_name(t_token *token);

//main_deub.c
void		generate_ast_diagram(t_ast_node *root);

//wildcard.c  for bonus but using here for testing
int			contains(char *str, char target);
t_token		*expand_wildcard(char *input, t_env **env,
				t_token *prev, t_token *next);
t_token		*new_word_token(char *str);
t_list		*expand_args(t_env **env, char *pattern, t_token *prev);

//wildcard_utils.c
t_list		*new_dir_entry(char *entry_name);
void		free_dir(t_env **env);
void		get_current_dir(t_env **env);
void		args_to_tokens(t_list *args, t_token *next, t_token **head);

//builtin.c
void		execute_builtin(t_env **env, char **args);

//builtin_utils.c
int			is_assignment(char *cmd);
int			is_builtin(char *cmd);

//builtin_cd.c
int			change_dir(t_env **env, char **args);

//env_var
void		add_var(char *var, char ***vars);
void		assign_variable(t_env *env, char ***vars, char *string);
int			export_var(t_env *env, char **args);
char		*get_var(char *env_var, char **vars);
char		*make_var(char *var, char *val);
void		remove_var(char *name, char ***vars);
int			valid_env_name(char *name);

//prompt.c
char		*prompt(t_env *env);
void		get_host_and_user(t_env *env);

//utils.c
int			contains(char *str, char target);
void		error_msg(char *cmd, int error_value);
int			is_dollar_special_case(char c);
void		put_3(char *s1, char *s2, char *s3);
int			is_dollar_special_case(char c);

// ast_visualizer.c
/*
void		print_escaped(FILE *stream, const char *str);
void		print_ast_dot(t_ast_node *node, FILE *stream);
void		generate_ast_diagram(t_ast_node *root);
*/

#endif

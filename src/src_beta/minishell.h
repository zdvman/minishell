/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samoore <samoore@student.42london.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 13:35:34 by samoore           #+#    #+#             */
/*   Updated: 2024/04/12 13:36:00 by samoore          ###   ########.fr       */
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
#include <dirent.h>

typedef enum
{	
	WORD,
	COMMAND,
	REDIR_IN,
	REDIR_IN_DBL,
	REDIR_OUT,
	REDIR_OUT_DBL,
	PIPE,
	CONTROL,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	EXIT,
	DONE,
	END
} t_token_type;

typedef struct s_redirect
{
	struct s_redirect		*next;
	t_token_type			type;
	char					*file;
} t_redirect;


typedef struct s_list
{
	struct s_list	*next;
	char			*entry;
}	t_list;

typedef struct s_token
{
	struct s_token *left;
	struct s_token *right;
	t_token_type	type;
	char			*string;
	int				val;
	t_list			*args_list;
	t_list			*args_head;
	char			**args;
	char			*command_path;
	int				pipe;
	char			control;
	t_redirect		*redirects;
	t_redirect		*redirects_head;
}	t_token;

typedef struct s_env
{
	char	**paths;
	char	**env;
	t_token	*tokens;
	t_token	*token_head;
	t_list	*directory_list;
	t_list	*dir_head;
	int		exit_status;
}	t_env;

void	add_path(t_env *env);
void	add_slash(char **paths);
void	check_pipes(t_env *env);
void	clear_env(t_env *env);
void	free_paths(t_env *env);
void	ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_memset(void *str, int c, size_t len);
char	**ft_split(char const *s, char c);
char	*ft_strdup(const char *str);
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlcat(char *dst, const char *src, size_t dstsize);
size_t	ft_strlcpy(char *dest, const char *src, size_t size);
size_t	ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
void	get_current_dir(t_env *env);
char	*get_path(t_env *env);
t_token	*get_tokens(char *line, int start, int len);
int		is_bracket(char c);
int		is_control(char c);
int		is_meta(char c);
int		is_quote(char c);
int		is_redirect(char c);
int		is_space(char c);
int		is_word(char c);
void	link_tokens_left(t_env *env);
int		parse_tokens(t_env *env);
void	print_tokens(t_token *tokens);
void	reset_tokens(t_env *env);

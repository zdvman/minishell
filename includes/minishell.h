/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuiev <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 10:29:58 by dzuiev            #+#    #+#             */
/*   Updated: 2024/04/10 15:55:07 by dzuiev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>


# define PROMPT "minishell$ "
# define MAX_CMD 1024

typedef struct	s_cmd
{
	char		*cmd;
	char		**args;
}				t_cmd;

void			init_shell(void);
void			prompt(void);
void			parse_cmd(char *cmd);
void			exec_cmd(t_cmd *cmd);
void			free_cmd(t_cmd *cmd);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 23:38:23 by moodeh            #+#    #+#             */
/*   Updated: 2026/01/05 01:26:06 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# define FALSEE 0
# define TRUEE 1
# include "libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

// alloc envp in child and free it in child dont bring the error to parent
typedef struct s_pipex
{
	pid_t	pid1;
	pid_t	pid2;
	int		my_pipe[2];
}			t_pipex;
typedef struct s_process_data
{
	int		fd;
	char	**cmd_have_flag;
	char	*path_that_have_cmd;
	char	**envp_path;
	int		there_is_error;
	int		casee;
}			t_process;
typedef struct s_names
{
	char	*full_path;
	char	*cmd_slash;
	int		i;
}			t_names;

void		process_1(t_pipex *pipex1, char *argv[], char *envp[]);
void		process_2(t_pipex *pipex1, char *argv[], char *envp[]);
void		error_handle_custom(char *ar, t_pipex *pipex1, int exit_code);
void		error_handle(char *ar, t_pipex *pipex1, int exit_code);
char		*check_path(char *cmd, char **envp);
char		**find_path(char **envp);

#endif
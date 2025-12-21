/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 23:38:23 by moodeh            #+#    #+#             */
/*   Updated: 2025/12/17 04:05:31 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# define FALSEE 0
# define TRUEE 1
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <libft.h>
void	process_1(int my_pipe[], char *argv[], char *envp[]);
void	process_2(int my_pipe[], char *argv[], char *envp[]);
int	checker(char *argv[] ,char **just_freeit,int casee);
void	error_handle(char *ar, int my_pipe[]);
#endif
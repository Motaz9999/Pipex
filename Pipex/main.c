/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 06:40:39 by moodeh            #+#    #+#             */
/*   Updated: 2026/01/05 23:17:25 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// ok first  command file1 cmd1 cmd2 file2
// infile "ls -l" "wc -l" this done to file one
// command 1 done to file1 then | to the next command
// then the output of these 2 cmd to the file2
// put here the trash you want to clean
void	error_handle_custom(char *ar, t_pipex *pipex1, int exit_code)
{
	if (pipex1 != NULL)
	{
		if (pipex1->my_pipe[0] > 0)
			close(pipex1->my_pipe[0]);
		if (pipex1->my_pipe[1] > 0)
			close(pipex1->my_pipe[1]);
	}
	ft_putstr_fd("ERROR : ", 2);
	ft_putstr_fd(ar, 2);
	ft_putstr_fd("\n", 2);
	exit(exit_code);
}

void	error_handle(char *ar, t_pipex *pipex1, int exit_code)
{
	if (pipex1 != NULL)
	{
		if (pipex1->my_pipe[0] > 0)
			close(pipex1->my_pipe[0]);
		if (pipex1->my_pipe[1] > 0)
			close(pipex1->my_pipe[1]);
	}
	ft_putstr_fd("ERROR : ", 2);
	perror(ar);
	ft_putstr_fd("\n", 2);
	exit(exit_code);
}
// waitpid(pid, NULL, 0);
// we want to wait for the child to finish
// 0-> wait normally it return the pid of the child that finished or
//-1 if it fails
// after finish we want to clean up
// in this fun we want to wait at least to both proccess to finish
// and we want to check the status and what if it exit norm or not
// we use WIFEXITED-> did the child finish norm ?
// WEXITSTATUS()->what is the return val/exit code if it
// finish norm(no seg fault

static int	parent(t_pipex *pipex1)
{
	int	status;
	int	exit_cause;

	if (pipex1->my_pipe[0] > 0)
		close(pipex1->my_pipe[0]);
	if (pipex1->my_pipe[1] > 0)
		close(pipex1->my_pipe[1]);
	waitpid(pipex1->pid1, NULL, 0);
	waitpid(pipex1->pid2, &status, 0);
	if (WIFEXITED(status))
		exit_cause = WEXITSTATUS(status);
	else
		exit_cause = 1;
	return (exit_cause);
}
// static  int parse_args(t_pipex *new_pipex, char *argv[])
//{//first we need to check on infile and outfile
// and creat them if i dont have them(outfile) and check if it readble
// i know i have at least 5 args soo i need to check
// them one by one
// first we start with argv[1] soo is it file?
// is it exist and open with me
// first check if it name ?
// fd = open(argv[1] , O_RDONLY);//if it return
//      -1 i cant handle it it is the problem of first child
// if (fd == -1)
//              return ;
// so the outfile
// fd = open(argv[4] , O_WRONLY | O_CREAT | O_TRUNC);//if it return
//      -1 i cant handle it it is the problem of second child
// if (fd == -1)
//              return ;
// so after the infiile outfile we need to check for cmd
//}
// make sure it takes flag with cmd like "ls -l"

int	main(int argc, char *argv[], char *envp[])
{
	t_pipex	pipex1;

	ft_bzero(&pipex1, sizeof(pipex1));
	if (argc != 5)
		error_handle_custom("invalid num of argument", NULL, 1);
	if (pipe(pipex1.my_pipe) == -1)
		error_handle("Pipe failed", NULL, 1);
	pipex1.pid1 = fork();
	if (pipex1.pid1 == -1)
		error_handle("fork failed", &pipex1, 1);
	else if (pipex1.pid1 == 0)
		process_1(&pipex1, argv, envp);
	pipex1.pid2 = fork();
	if (pipex1.pid2 == -1)
		error_handle("fork failed", &pipex1, 1);
	else if (pipex1.pid2 == 0)
		process_2(&pipex1, argv, envp);
	return (parent(&pipex1));
}

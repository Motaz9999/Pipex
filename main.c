/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 06:40:39 by moodeh            #+#    #+#             */
/*   Updated: 2025/12/27 06:40:42 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

		// ok first  command file1 cmd1 cmd2 file2
		// infile "ls -l" "wc -l" this done to file one
		// command 1 done to file1 then | to the next command
		// then the output of these 2 cmd to the file2
		// put here the trash you want to clean
		void error_handle(char *ar, pipex_t *pipex1)
{
	if (pipex1 != NULL)
	{
		if (pipex1->my_pipe[0] > 0)
			close(pipex1->my_pipe[0]);
		if (pipex1->my_pipe[1] > 0)
			close(pipex1->my_pipe[1]);
	}
	perror(ar);
	exit(1);
}
// waitpid(pid, NULL, 0);
// we want to wait for the child to finish  0-> wait normally it return the pid of the child that finished or
//-1 if it fails
// after finish we want to clean up
// in this fun we want to wait at least to both proccess to finsh
// and we want to check the status and what if it exit norm or not
// we use WIFEXITED-> did the chiled finish norm ?
// WEXITSTATUS()->what is the return val/exit code if it finish norm(no seg fault)
static int	parent(pipex_t *pipex1)
{
	int	exit_cause;
	int	status;

	code Code if (pipex1->my_pipe[0] > 0) close(pipex1->my_pipe[0]);
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
// static  int parse_args(pipex_t *new_pipex, char *argv[])
//{//first we need to check on infile and outfile and creat them if i dont have them(outfile) and check if it readble
// i know i have at least 5 args soo i need to check them one by one
// first we start with argv[1] soo is it file? is it exisit and open with me
// first check if it name ?
// fd = open(argv[1] , O_RDONLY);//if it return
//      -1 i cant handle it it is the problem of first chiled
// if (fd == -1)
//              return ;
// so the outfile
// fd = open(argv[4] , O_WRONLY | O_CREAT | O_TRUNC);//if it return
//      -1 i cant handle it it is the problem of secound chiled
// if (fd == -1)
//              return ;
// so after the infiile outfile we need to check for cmd
//}
// make sure it takes falge with cmd like "ls -l"
int	main(int argc, char *argv[], char *envp[])
{
	pipex_t pipex1;
	ft_memset(&pipex1, 0, sizeof(pipex1));
	// so here all args in struct = 0
	// here we make the pipe
	if (argc != 5 || pipe(pipex1.my_pipe) == -1)
		error_handle("error in the argv argc ,envp,pipe failed", NULL);
	// soo we need to check if the envp have PATH and Check for infile and outfile and cmds inside chileds
	pipex1.pid1 = fork();
	if (pipex1.pid1 == -1)
		error_handle("error fork first child", &pipex1);
	else if (pipex1.pid1 == 0)
		process_1(&pipex1, argv, envp);
	pipex1.pid2 = fork();
	if (pipex1.pid2 == -1)
		error_handle("error fork second child", &pipex1);
	else if (pipex1.pid2 == 0)
		process_2(&pipex1, argv, envp);
	return (parent(&pipex1));
}
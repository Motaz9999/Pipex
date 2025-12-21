/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 23:38:20 by moodeh            #+#    #+#             */
/*   Updated: 2025/12/17 04:32:00 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// ok first  command file1 cmd1 cmd2 file2
// infile "ls -l" "wc -l" this done to file one
// command 1 done to file1 then | to the next command
// then the output of these 2 cmd to the file2

// put here the trash you want to clean
void	error_handle(char *ar, int my_pipe[])
{
	if (my_pipe != NULL)
	{
		close(my_pipe[0]);
		close(my_pipe[1]);
	}
	perror(ar);
	exit(1);
}

// HERE checks to diff cases
int	checker(char *argv[], char **just_freeit, int casee)
{
	int	fd;

	if (casee == 1)
	{
		if (argv[2][0] == "") // string is empty
			return (FALSEE);
		close(fd);
		if (just_freeit == NULL || just_freeit[0] == NULL)
			return (FALSEE);
	}
	else if ((casee == 2) && (just_freeit[0] == NULL))
		// this is for check on cmd
	{
		ft_free_all2(just_freeit);
		return (FALSEE);
	}
	return (TRUEE);
}

// waitpid(pid, NULL, 0);
// we want to wait for the child to finish  0-> wait normally it return the pid of the child that finished or
//-1 if it fails
// after finish we want to clean up
// in this fun we want to wait at least to both proccess to finsh
// and we want to check the status and what if it exit norm or not
// we use WIFEXITED-> did the chiled finish norm ?
// WEXITSTATUS()->what is the return val/exit code if it finish norm(no seg fault)
static int	parent(pid_t pid1, pid_t pid2)
{
	int	exit_cause;
	int	status;

	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);
	if (WIFEXITED(status))
		exit_cause = WEXITSTATUS(status);
	else
		exit_cause = 1;
	return (exit_cause);
}

// make sure it takes falge with cmd like "ls -l"
int	main(int argc, char *argv[], char *envp[])
{
	pid_t	pid1;
	pid_t	pid2;
	int		my_pipe[2];

	if (argc < 5 || envp == NULL || !checker(argv, envp, 1) || pipe(my_pipe) ==
		-1)
		error_handle("error in the argv argc or envp or pipe", NULL);
	pid1 = fork(); // after fork we check
	if (pid1 < 0)
		error_handle("error in pid1", my_pipe);
	else if (pid1 == 0)
		process_1(my_pipe, argv, envp);
	pid2 = fork();
	if (pid2 < 0)
		error_handle("error in pid2", my_pipe);
	if (pid2 == 0)
		process_2(my_pipe, argv, envp);
	close(my_pipe[0]);
	close(my_pipe[1]);
	// here we close them in main then we close them in each process
	return (parent(pid1, pid2));
}

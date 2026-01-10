/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 19:26:02 by moodeh            #+#    #+#             */
/*   Updated: 2026/01/05 01:26:08 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// pid = 0 -> child the sys give the child always 0 (Fork output)
// pid > 0 -> parent (the original pid from sys) its like supervisor and
// wait for child to finish using waitpid(id);
// pid < 0 -> the my_pipe failed and here we want to fix and exit
// so first in child we need to check on the dictionary (envp)
// NOW i have the full paths soo i must see if the cmd is having flags

static void	helper(t_pipex *pipex1, t_process *pro1, char *argv[], char *envp[])
{
	pro1->envp_path = find_path(envp);
	pro1->cmd_have_flag = ft_split(argv[pro1->casee], ' ');
	if (!pro1->cmd_have_flag || !pro1->cmd_have_flag[0])
	{
		if (pro1->envp_path != NULL && pro1->envp_path[0] != NULL)
			ft_free_all2((void **)pro1->envp_path, NULL);
		if (pro1->cmd_have_flag != NULL)
			ft_free_all2((void **)pro1->cmd_have_flag, NULL);
		if (pro1->fd != -1)
			close(pro1->fd);
		error_handle_custom("command not found", pipex1, 127);
	}
	pro1->path_that_have_cmd = check_path(pro1->cmd_have_flag[0],
			pro1->envp_path);
	if (pro1->path_that_have_cmd == NULL)
	{
		if (pro1->envp_path != NULL && pro1->envp_path[0] != NULL)
			ft_free_all2((void **)pro1->envp_path, NULL);
		if (pro1->cmd_have_flag != NULL && pro1->cmd_have_flag[0] != NULL)
			ft_free_all2((void **)pro1->cmd_have_flag, NULL);
		if (pro1->fd != -1)
			close(pro1->fd);
		error_handle_custom("command not found", pipex1, 127);
	}
}
// before anything i should check on the infile

void	process_1(t_pipex *pipex1, char *argv[], char *envp[])
{
	t_process	pro1;

	ft_bzero(&pro1, sizeof(pro1));
	pro1.fd = open(argv[1], O_RDONLY);
	if (pro1.fd == -1)
		error_handle(argv[1], pipex1, 1);
	pro1.casee = 2;
	helper(pipex1, &pro1, argv, envp);
	if (dup2(pro1.fd, 0) == -1 || dup2(pipex1->my_pipe[1], 1) == -1)
	{
		close(pro1.fd);
		error_handle("dup2 failed", pipex1, 1);
	}
	close(pro1.fd);
	close(pipex1->my_pipe[0]);
	close(pipex1->my_pipe[1]);
	pro1.there_is_error = execve(pro1.path_that_have_cmd, pro1.cmd_have_flag,
			envp);
	ft_free_all((void *)pro1.path_that_have_cmd, NULL);
	ft_free_all2((void **)pro1.cmd_have_flag, (void **)pro1.envp_path, NULL);
	if (pro1.there_is_error == -1)
		error_handle("Execve failed", pipex1, 1);
	else
		exit(1);
}

// O_WRONLY ->just read | O_CREAT -> creat if it doesn't exist | O_TRUNC
//	->clean it if it have any input
void	process_2(t_pipex *pipex1, char *argv[], char *envp[])
{
	t_process	pro2;

	ft_bzero(&pro2, sizeof(pro2));
	pro2.fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pro2.fd == -1)
		error_handle(argv[4], pipex1, 1);
	pro2.casee = 3;
	helper(pipex1, &pro2, argv, envp);
	if ((dup2(pro2.fd, 1)) == -1 || (dup2(pipex1->my_pipe[0], 0)) == -1)
	{
		close(pro2.fd);
		error_handle("dup2 failed", pipex1, 1);
	}
	close(pro2.fd);
	close(pipex1->my_pipe[1]);
	close(pipex1->my_pipe[0]);
	pro2.there_is_error = execve(pro2.path_that_have_cmd, pro2.cmd_have_flag,
			envp);
	ft_free_all((void *)pro2.path_that_have_cmd, NULL);
	ft_free_all2((void **)pro2.cmd_have_flag, (void **)pro2.envp_path, NULL);
	if (pro2.there_is_error == -1)
		error_handle("Execve failed", pipex1, 1);
	else
		exit(1);
}

// for the pipes and fds we want to link
// them all (link t_process with each other)
// first t_process its link the input
// with infile and the output with my_pipe[1]
//	->enter
// sec t_process link the my_pipe[0] with
// input and the output with outfile soo thats it
// then we clean all
// btw if execve works it del everything
// so dont worry for the leaks like the cmd

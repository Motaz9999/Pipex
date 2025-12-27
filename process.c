#include "pipex.h"

// pid = 0 -> child the sys give the child always 0 (Fork output)
// pid > 0 -> parent (the orginal pid from sys) its like supervisor and
// wait for child to finish using waitpid(id);
// pid < 0 -> the my_pipe faild and here we want to fix and exit
static void	helper(pipex_t *pipex1, process *pro1, char *argv[], char *envp[])
{
	// so first in child we need to check on the dictoinary (envp)
	pro1->envp_path = find_path(envp);
	// NOW i have the full paths soo i must see if the cmd is having flages
	pro1->cmd_have_flag = ft_split(argv[pro1->casee], ' '); // free it later
	if (!pro1->cmd_have_flag || !pro1->cmd_have_flag[0])
	{
		if (pro1->envp_path != NULL && pro1->envp_path[0] != NULL)
			ft_free_all2((void **)pro1->envp_path);
		error_handle("error cmd-split failed in pro1", pipex1);
	}
	// so now that  we have the cmd and everything we need why we dont check on the cmd in envp
	pro1->path_that_have_cmd = check_path(pro1->cmd_have_flag[0],
			pro1->envp_path);
	// here we give it the paths and the cmd soo it sohould check it
	if (pro1->path_that_have_cmd == NULL)
	{
		if (pro1->envp_path != NULL && pro1->envp_path[0] != NULL)
			ft_free_all2( (void **)pro1->envp_path);
		ft_free_all2((void **)pro1->cmd_have_flag);
		error_handle("error path that have cmd", pipex1);
	}
}
void	process_1(pipex_t *pipex1, char *argv[], char *envp[])
{
	process	pro1;

	ft_bzero(&pro1, sizeof(pro1));
	// before anything i should check on the infile
	pro1.fd = open(argv[1], O_RDONLY);
	if (pro1.fd == -1)
		error_handle("error cant open infile or it dosent exisit", pipex1);
	pro1.casee = 2;
	helper(pipex1, &pro1, argv, envp); // here we do all the work
	dup2(pro1.fd, 0);                  // this is my input
	dup2(pipex1->my_pipe[1], 1);       // this is my output
	close(pro1.fd);
	close(pipex1->my_pipe[0]);
	close(pipex1->my_pipe[1]);
	pro1.there_is_error = execve(pro1.path_that_have_cmd, pro1.cmd_have_flag,
			envp);
	ft_free_all((void *)pro1.path_that_have_cmd);
	ft_free_all2((void **)pro1.cmd_have_flag, (void **)pro1.envp_path);
	if (pro1.there_is_error == -1)
		error_handle("error in execve", pipex1);
	else
		exit(1);
}

// O_WRONLY ->just read | O_CREAT -> creat if it dosnt exist | O_TRUNC
//	->clean it if it have any input
void	process_2(pipex_t *pipex1, char *argv[], char *envp[])
{
	process	pro2;

	ft_bzero(&pro2, sizeof(pro2));
	pro2.fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pro2.fd == -1)
		error_handle("error cant open outfile", pipex1);
	pro2.casee = 3;
	helper(pipex1, &pro2, argv, envp); // here we do all the work
	dup2(pro2.fd, 1);                  // this is my output goes to
	dup2(pipex1->my_pipe[0], 0);       // this is my input come from
			close(pro2.fd);
	close(pipex1->my_pipe[1]);
	close(pipex1->my_pipe[0]);
	pro2.there_is_error = execve(pro2.path_that_have_cmd, pro2.cmd_have_flag,
			envp);
	ft_free_all((void *)pro2.path_that_have_cmd);
	ft_free_all2((void **)pro2.cmd_have_flag, (void **)pro2.envp_path);
	if (pro2.there_is_error == -1)
		error_handle(strerror(pro2.there_is_error), pipex1);
	else
		exit(1);
}

// for the pipes and fds we want to link them all (link process with each other)
// first process its link the input with infile and the output with my_pipe[1]
//	->enter
// sec process link the my_pipe[0] with input and the output with outfile soo thats it
// then we clean all
// btw if execve works it del everything so dont worry for the leaks like the cmd
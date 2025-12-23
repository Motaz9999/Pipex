#include "pipex.h"

// pid = 0 -> child the sys give the child always 0 (Fork output)
// pid > 0 -> parent (the orginal pid from sys) its like supervisor and
// wait for child to finish using waitpid(id);
// pid < 0 -> the my_pipe faild and here we want to fix and exit
void	process_1(int my_pipe[], char *argv[], char *envp[])
{
	int		fd;
	char	**cmd_have_flag;
	char	*path;

    fd = open(argv[1], O_RDONLY); // to take input
	if (fd < 0)
		error_handle("error Infile  not open\n", my_pipe);//here we print and exit form chiled
	// here we  want the child to 1.make pipes 2.run cmds   btw the childs here is 2cmd
	if ((cmd_have_flag = ft_split(argv[2], ' ')) == NULL || checker(NULL , cmd_have_flag ,2))
		error_handle("error in malloc for split to use for flag or cmd not found\n", my_pipe);
	close(my_pipe[0]);//i dont use it in the first process soo its ok
    dup2(fd, 0); // here we want to take input as fd == like == 0->deafult
    dup2(my_pipe[1], 1);
	// here we find the path  wait the cmd maybe be ls-l sooooo theres a spaces !!
	path = check_path(cmd_have_flag[0], envp);//soo lets close the reset after the link
	close(fd); // we did the lnik already so we dont need the fd
    close(my_pipe[1]);
	if (path == NULL || execve(path, cmd_have_flag, envp) == -1)
	{
		ft_free_all2((void**)cmd_have_flag);
		error_handle("error cant access or execv failed on falid dosent exisit", my_pipe);
	}
    ft_free_all2((void**)cmd_have_flag);//safe
}
//O_WRONLY ->just read | O_CREAT -> creat if it dosnt exist | O_TRUNC ->clean it if it have any input 
void	process_2( int my_pipe[], char *argv[], char *envp[])
{
	int		fd;
	char	**cmd_have_flag;
	char	*path;

    fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC , 0777); // to take input
	if (fd < 0)
		error_handle("error Outfile not open", my_pipe);
	if ((cmd_have_flag = ft_split(argv[3], ' ')) == NULL || checker(NULL , cmd_have_flag ,2))
		error_handle("error in malloc for split to use for flag", my_pipe);
    close(my_pipe[1]);
	dup2(my_pipe[0], 0);
	dup2(fd, 1);
	close(fd); // we did the lnik already so we dont need the fd
    close(my_pipe[0]);
    path = check_path(cmd_have_flag[0], envp);
	if (path == NULL || execve(path, cmd_have_flag, envp) == -1)
	{
		ft_free_all2((void**)cmd_have_flag);
		error_handle("error cant access or execv failed", my_pipe);
	}
    ft_free_all2((void**)cmd_have_flag);//just to be safe 
}

//for the pipes and fds we want to link them all (link process with each other)
//first process its link the input with infile and the output with my_pipe[1] ->enter
//sec process link the my_pipe[0] with input and the output with outfile soo thats it
//then we clean all 
//btw if execve works it del everything so dont worry for the leaks like the cmd
/* ************************************************************************** /
/                                                                            /
/                                                        :::      ::::::::   /
/   find_path.c                                        :+:      :+:    :+:   /
/                                                    +:+ +:+         +:+     /
/   By: moodeh moodeh@student.42.fr              +#+  +:+       +#+        /
/                                                +#+#+#+#+#+   +#+           /
/   Created: 2025/12/11 02:33:04 by moodeh            #+#    #+#             /
/   Updated: 2025/12/27 05:43:21 by moodeh           ###   ########.fr       /
/                                                                            /
/ ************************************************************************** */
#include "pipex.h"

// here we checked for abs path or
static char	*path_rel_abs_not_name(char *cmd)
{
	if (access(cmd, X_OK | F_OK) < 0)
	{
		printf("error file dosnt exisit or cant excute\n");
		return (NULL);
	}
	return (ft_strdup(cmd)); // is it full path and exisit and excutable
}
static int	is_it_realtive_or_abs(char *cmd)
{
	// here we check if it realtive or abs
	// first we check if it abs
	if (cmd[0] == '.')           // here it is relative cmd
		return (TRUEE);          // use it
	else if (cmd[0] == '/')      // it is abs
		return (TRUEE);          // use it
	else if (ft_isalnum(cmd[0])) // it is not abs and relaitve
		return (FALSEE);         // search on it
	return (FALSEE);             // something else
}
char	**find_path(char **envp)
{
	int		i;
	char	**path;


		// now that i hav a bunch of files i should find where the line that start with path and skip it
		// bc we have mlitple folders to search in it//now ltes find the file
	if (!envp || !envp[0]) 
        return (NULL);
	i = 0;
	while (ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	// ok here we search on the word PATH its 100% in envp soo no need to check if envp dont have it
	// now after i find the locat of "PATH" let cut into paths put before lets skip the "path="
	path = ft_split(envp[i] + 5, ':');
		// after we find the loca (i) give it+5 we dont need the "PATH="
	// ok now i have every path in my envyroment soo lets search it one by one
	if (!path || !path[0]) // check if split falied
		return (NULL);
	return (path);
}
// here we check if the cmd is ok access and path is ok  btw we dont need the flag so its ok
// F_OK File Existence. Does the file exist at all?
// X_OK Executable. Do we have permission to Run it? (Crucial for finding commands like ls).
// R_OK Readable. Do we have permission to Read it? (Crucial for infile).
// W_OK Writable. Do we have permission to Write to it? (Crucial for outfile).
// what if user inter the full path "/bin/ls"
char	*check_path(char *cmd, char *envp_path)
{
	t_names names;
	// first we need to check the path if it full or just name
	ft_bzero(&names, sizeof(names));
	if (is_it_realtive_or_abs(cmd) == 1)             // 1-> just go to check on it else continue on this
		return (path_rel_abs_not_name(cmd));        
			// here it retrun null or the path if it all good
	if ((envp_path == NULL || envp_path[0] == NULL))
		// here if you just have the name of the cmd
		return (NULL);
	if ((names.cmd_slash = ft_strjoin("/", cmd)) == NULL)
		return (NULL);
	while (envp_path && envp_path[names.i])
		// here we check on the cmd if it is exisit in envp path_by_path
	{
		names.full_path = ft_strjoin(envp_path[names.i], names.cmd_slash);
		if (names.full_path) // here we check if the join failed
		{
			if (access(names.full_path, F_OK | X_OK) == 0)
			{
				free(names.cmd_slash);    // i dont need it anymore
				return (names.full_path); // just return it
			}
			free(names.full_path); // so i can reuse it
		}
		names.i++;
	}
	free(names.cmd_slash);
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 02:33:04 by moodeh            #+#    #+#             */
/*   Updated: 2025/12/17 04:03:41 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	is_it_full_path(char *cmd)
{
	if (access(cmd, F_OK) < 0)
	{
		printf("error file dosnt exisit\n");
		return (FALSEE);
	}
	if (access(cmd, F_OK) < 0)
	{
		printf("error file dosnt excute\n");
		return (FALSEE);
	}
	return (TRUEE); // is it full path and exisit and excutable
}

char	*find_path(char *cmd, char **envp)
{
	int		i;
	char	**path;

	// now that i hav a bunch of files i should find where the line that start with path and skip it
	// bc we have mlitple folders to search in it//now ltes find the file
	i = 0;
	while (ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	// now after i find the locat of "PATH" let cut into paths put before lets skip the "path="
	path = ft_split(envp[i] + 5, ':');
	// ok now i have every path in my envyroment soo lets search it one by one
	if (path == NULL)
		return (NULL);
	return (path);
}
// here we check if the cmd is ok access and path is ok  btw we dont need the flag so its ok
// F_OK	File Existence. Does the file exist at all?
// X_OK	Executable. Do we have permission to Run it? (Crucial for finding commands like ls).
// R_OK	Readable. Do we have permission to Read it? (Crucial for infile).
// W_OK	Writable. Do we have permission to Write to it? (Crucial for outfile).

// what if user inter the full path "/bin/ls"
int	check_path(char *cmd, char **envp)
{
	char	**path;
	char	*full_path;
	char	*full_name;
	int		i;

	i = 0;
	if (is_it_full_path(cmd))
		return (check_full_path(cmd, envp));
	if ((full_name = ft_strjoin("/", cmd)))
		return (FALSEE);
	path = find_path(cmd, envp); // here we find all paths(folder)
	while (path[i++] != NULL)
	{
		if ((full_path = ft_strjoin(path, full_name)) == NULL)
		{
			free(full_name);
			return (FALSEE);
		}
		if (access(full_path, F_OK | X_OK) == 0)
		{
			ft_free_all(full_name, full_path);
			return (full_path);//found the path 
		}
	}
	return (NULL);
}

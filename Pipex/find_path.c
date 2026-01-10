/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moodeh <moodeh@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 19:22:11 by moodeh            #+#    #+#             */
/*   Updated: 2026/01/05 01:28:18 by moodeh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// here we checked for abs path or
// is it full path and exist and executable
static char	*path_rel_abs_not_name(char *cmd)
{
	char	*to_return;

	if (access(cmd, X_OK | F_OK) < 0)
		return (NULL);
	to_return = ft_strdup(cmd);
	if (to_return == NULL)
		return (NULL);
	return (to_return);
}
// here we check if it relative or abs
// first we check if it abs

static int	is_it_relative_or_abs(char *cmd)
{
	if (cmd[0] == '.')
		return (TRUEE);
	else if (cmd[0] == '/')
		return (TRUEE);
	else if (ft_isalnum(cmd[0]))
		return (FALSEE);
	return (FALSEE);
}
// now that i hav a bunch of files i should find where the
// line that start with path and skip it
// bc we have mutiple folders to search in it//now lets find the file
// ok here we search on the word PATH its 100%
// in envp soo no need to check if envp dont have it
// now after i find the local of "PATH"
// let cut into paths put before lets skip the "path="
// after we find the local (i) give it+5 we dont need the "PATH="
// ok now i have every path in my environment soo lets
// search it one by one

char	**find_path(char **envp)
{
	int		i;
	char	**path;

	if (!envp || !envp[0])
		return (NULL);
	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (NULL);
	path = ft_split(envp[i] + 5, ':');
	if (!path || !path[0])
		return (NULL);
	return (path);
}
// here we check if the cmd is ok access and path
// is ok  btw we dont need the flag so its ok
// F_OK File Existence. Does the file exist at all?
// X_OK Executable. Do we have permission to Run it?
// (Crucial for finding commands like ls).
// R_OK Readable. Do we have permission to Read it? (Crucial for infile).
// W_OK Writable. Do we have permission to Write to it? (Crucial for outfile).
// what if user inter the full path "/bin/ls"

char	*check_path(char *cmd, char **envp_path)
{
	t_names	names;

	ft_bzero(&names, sizeof(names));
	if (is_it_relative_or_abs(cmd) == 1)
		return (path_rel_abs_not_name(cmd));
	if ((envp_path == NULL || envp_path[0] == NULL))
		return (NULL);
	names.cmd_slash = ft_strjoin("/", cmd);
	if (names.cmd_slash == NULL)
		return (NULL);
	while (envp_path && envp_path[names.i])
	{
		names.full_path = ft_strjoin(envp_path[names.i++], names.cmd_slash);
		if (names.full_path)
		{
			if (access(names.full_path, F_OK | X_OK) == 0)
			{
				free(names.cmd_slash);
				return (names.full_path);
			}
			free(names.full_path);
		}
	}
	free(names.cmd_slash);
	return (NULL);
}

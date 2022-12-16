#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int	ft_strlen(char *str)
{
	int i;

	if (str == NULL)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_exec(char **av, int i, char **env)
{
	int j;
	int fd[2];
	int pid;
	int tmp;

	j  = 0;
	tmp = dup(0);//here
	av[i] = NULL;
	while (av[j])
	{
		i = j;
		while (av[j] && strcmp(av[j], "|") && strcmp(av[j], ";"))
			j++;
		if (av[j] && strcmp(av[j], "|") == 0)
		{
			if (pipe(fd) == -1)
			{
				write(2, "error: fatal\n", 13);
				exit(1);
			}
		}
		pid = fork();
		if (pid < 0)
		{
			write(2, "error: fatal\n", 13);
			exit(1);
		}
		if (pid == 0)
		{
			if (av[j] && strcmp(av[j], "|") == 0)
			{
				dup2(fd[1], 1);//here
				close(fd[0]);
				close(fd[1]);
			}
			av[j] = NULL;
			dup2(tmp, 0);//here
			close(tmp);
			execve(av[i], &av[i], env);
			write(2, "error: cannot execute ", 22);
			write(2, av[i], ft_strlen(av[i]));
			write(2, "\n", 1);
			exit(1);
		}
		else
		{
			close(fd[1]);
			close(tmp);
			tmp = fd[0];
			if (av[j] != NULL)
				j++;
		}
	}
	while (waitpid(-1, NULL, 0) != -1)
	;
	close(tmp);
}

void	ft_cd(char **av, int i)
{
	if (i != 2)
		write(2, "error: cd: bad arguments\n", 25);
	else if (chdir(av[1]))
	{
		write(2, "error: cd: cannot change directory to ", 38);
		write(2, av[1], ft_strlen(av[1]));
		write(2, "\n", 1);
	}	
}

int	main(int ac, char **av, char **env)
{
	int	i;
	char	*tmp;

	if (ac == 1)
		return (0);
	av = &av[1];
	i = 0;
	while (av[i])
	{	
		while (av[i] && strcmp(av[i], ";"))
			i++;
		if (i != 0 && strcmp(av[0], "cd") == 0)
			ft_cd(av, i);
		else if (i != 0)
		{
			tmp = av[i];
			ft_exec(av, i, env);
			av[i] = tmp;
		}
		if ((i != 0 && av[i] != NULL) || (av[i] != NULL && strcmp(av[i], ";") == 0))
		{
			av = &av[i + 1];
			i = 0;
		}
	}
	return (0);
}

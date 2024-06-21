/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jveirman <jveirman@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 10:33:13 by jveirman          #+#    #+#             */
/*   Updated: 2024/06/12 12:55:30 by jveirman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/exec.h"

static void	here_doc_found(t_shell *shell, int i);

/*
* INFO:
*	Checks for each command whether one or more 
*	here_doc_delimiter delimiters are defined.
*/
void	here_doc_management(t_shell *shell)
{
	int	i;

	i = 0;
	while (i < shell->cmd_number)
	{
		if (!shell->cmd_array[i].here_doc_delimiter[0])
		{
			printf("NO_HERE_DOC\n");
			i++;
			continue ;
		}
		printf("HERE_DOC from cmd_array[%i]\n", i); //debug
		here_doc_found(shell, i);
		i++;
	}
}

/*
* INFO:
*	we will need to open the terminal for every here doc
*	And loop through until it reaches the last one.
*	When at the last index of delimiter, it will store it
*	to the here_doc_in string variable.
*	Then during the cmd execution, if the cmd has this string not set
*	to NULL, then we will use this string has an input.
*/
static void	here_doc_found(t_shell *shell, int i)
{
	int		j;
	char	**delimiter;

	j = 0;
	delimiter = shell->cmd_array[i].here_doc_delimiter;
	while (delimiter[j])
	{
		printf("CURRENT DELIMITER [%s]\n", delimiter[j]);//debug
		if (shell->cmd_array[i].here_doc_in)
			free(shell->cmd_array[i].here_doc_in);
		shell->cmd_array[i].here_doc_in = NULL;
		shell->cmd_array[i].here_doc_in = to_the_delimiter(delimiter[j]);
		if (!shell->cmd_array[i].here_doc_delimiter[j + 1])
			break ;
		j++;
	}
	// ft_arrayfree(shell->cmd_array[i].here_doc_delimiter); // debug: dev mode has hardcoded array, decomment for prod
}

void	here_doc_exploit(t_shell *shell, int i)
{
	int		pipe_fd[2];
	pid_t	pid;
	char	*text_input;

	if (0 > pipe(pipe_fd))
		exit(EXIT_FAILURE);
	pid = fork();
	if (-1 == pid)
		exit(EXIT_FAILURE);
	if (pid)
	{
		close(pipe_fd[1]);
		dup2(pipe_fd[0], STDIN_FILENO);
		waitpid(pid, NULL, 0);
	}
	if (0 == pid)
	{
		close(pipe_fd[0]);
		text_input = shell->cmd_array[i].here_doc_in;
		ft_putstr_fd(text_input, pipe_fd[1]);
		free(text_input); // wip: double check here
		close(pipe_fd[1]);
		exit(EXIT_SUCCESS);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int	*generate_random_array(int size)
{
	int	*array;
	int	i, j, temp;
	static int r = 0;

	array = malloc(sizeof(int) * size);
	if (!array)
		return (NULL);

	for (i = 0; i < size; i++)
		array[i] = i + 1;

	srand(time(NULL) + r++);
	for (i = size - 1; i > 0; i--)
	{
		j = rand() % (i + 1);
		temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
	array[0] = 0;

	return (array);
}

void	add_str_arg(char **args, char *str)
{
	while (*args)
		args++;
	*args = str;
}

void	add_int_tab_args(char **args, int *array, int size)
{
	int		i;
	char	*str;

	while (*args)
		args++;
	for (i = 0; i < size; i++)
	{
		str = malloc(sizeof(char) * 12);
		if (!str)
			return ;
		sprintf(str, "%d", array[i]);
		args[i] = str;
	}
}

void	exec_push_swap(char **args)
{
	int pipefd[2];
	pid_t pid;

	if (pipe(pipefd) == -1) {
		perror("Erreur lors de la création du pipe");
		return;
	}

	pid = fork();
	if (pid == -1) {
		perror("Erreur lors de la création du processus");
		return;
	}

	if (pid == 0) {
		// Processus enfant
		close(pipefd[0]); // Fermer le côté lecture
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]); // Fermer le côté écriture après redirection

		execvp("./push_swap", args);
		perror("Erreur lors de l'exécution de ./push_swap");
		exit(EXIT_FAILURE);
	} else {
		// Processus parent
		close(pipefd[1]); // Fermer le côté écriture

		int filefd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (filefd == -1) {
			perror("Erreur lors de l'ouverture du fichier");
			close(pipefd[0]);
			return;
		}

		char buffer[128];
		ssize_t bytesRead;

		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			if (write(filefd, buffer, bytesRead) == -1) {
				perror("Erreur lors de l'écriture dans le fichier");
				close(pipefd[0]);
				close(filefd);
				return;
			}
		}

		// Nettoyer
		close(pipefd[0]);
		close(filefd);

		// Attendre la fin de l'enfant
		wait(NULL);
	}
}

int	exec_checker(char **args)
{
	int pipefd[2]; // Pipe pour capturer la sortie de checker
	pid_t pid;

	if (pipe(pipefd) == -1) {
		perror("Erreur lors de la création du pipe");
		return -1;
	}

	pid = fork();
	if (pid == -1) {
		perror("Erreur lors de la création du processus");
		return -1;
	}

	if (pid == 0) {
		// Processus enfant
		close(pipefd[0]); // Fermer l'extrémité lecture du pipe

		// Rediriger la sortie standard vers le pipe
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]); // Fermer l'extrémité écriture après redirection

		// Ouvrir le fichier output.txt et rediriger son contenu vers stdin
		int filefd = open("output.txt", O_RDONLY);
		if (filefd == -1) {
			perror("Erreur lors de l'ouverture de output.txt");
			exit(EXIT_FAILURE);
		}
		dup2(filefd, STDIN_FILENO);
		close(filefd); // Fermer le fichier après redirection

		// Exécuter checker
		execvp("./checker", args);
		perror("Erreur lors de l'exécution de ./checker");
		exit(EXIT_FAILURE);
	} else {
		// Processus parent
		close(pipefd[1]); // Fermer l'extrémité écriture

		// Lire et capturer la sortie du processus enfant
		char buffer[128];
		ssize_t bytesRead;

		wait(NULL);
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			if (memcmp(buffer, "KO", 2) == 0) {
				close(pipefd[0]);
				return 1;
			}
			if (memcmp(buffer, "OK", 2) == 0) {
				close(pipefd[0]);
				return 0;
			}
			if (memcmp(buffer, "Error", 5) == 0) {
				close(pipefd[0]);
				return -1;
			}
		}

		close(pipefd[0]); // Fermer l'extrémité lecture
		return -1;
	}
	return -1;
}

int	count_lines(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		perror("Erreur lors de l'ouverture du fichier");
		return -1;
	}

	int lines = 0;
	int c;

	while ((c = fgetc(file)) != EOF) {
		if (c == '\n') {
			lines++;
		}
	}

	fclose(file);
	return lines;
}


int main() {
	int	*array;
	int	min;
	int	max;

	for (int i = 1; i <= 500; i++) {
		printf("Testing with \033[35m%d\033[0m elements :\n", i);
		min = -1;
		max = -1;
		for (int j = 0; j < 10; j++)
		{
			array = generate_random_array(i);
			if (!array)
				return (1);

			char	*args[502] = { "./push_swap", NULL };
			add_int_tab_args(args, array, i);
			exec_push_swap(args);

			int nb_lines = count_lines("output.txt");
			if (min == -1 || nb_lines < min)
				min = nb_lines;
			if (max == -1 || nb_lines > max)
				max = nb_lines;

			args[0] = "./checker";
			switch (exec_checker(args))
			{
			case -1:
				write(1, "\033[31m[Error]\033[0m ", 18);
				break;
			case 0:
				write(1, "\033[32m[OK]\033[0m ", 15);
				break;
			case 1:
				write(1, "\033[32m[KO]\033[0m ", 15);
				break;
			default:
				break;
			}

			free(array);
		}
		write(1, "\n", 1);
		printf("Min: %d, Max: %d\n\n", min, max);
	}

	return 0;
}

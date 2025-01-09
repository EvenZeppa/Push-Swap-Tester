#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

// Define color codes for console output
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define PURPLE "\033[35m"
#define RESET "\033[0m"

// Function to generate a random array of integers
int *generate_random_array(int size) {
	int *array;
	int i;
	static int r = 0;

	array = malloc(sizeof(int) * size);
	if (!array)
		return (NULL);

	srand(time(NULL) + r++);
	for (i = size - 1; i >= 0; i--)
		array[i] = rand() - RAND_MAX / 2;
	return (array);
}

// Function to add a string argument to an array of arguments
void add_str_arg(char **args, char *str) {
	while (*args)
		args++;
	*args = str;
}

// Function to add integer array elements as string arguments
void add_int_tab_args(char **args, int *array, int size) {
	int i;
	char *str;

	while (*args)
		args++;
	for (i = 0; i < size; i++) {
		str = malloc(sizeof(char) * 12);
		if (!str)
			return;
		sprintf(str, "%d", array[i]);
		args[i] = str;
	}
}

// Function to execute the push_swap program
void exec_push_swap(char **args) {
	int pipefd[2];
	pid_t pid;

	if (pipe(pipefd) == -1) {
		perror("Error creating pipe");
		return;
	}

	pid = fork();
	if (pid == -1) {
		perror("Error creating process");
		return;
	}

	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		execvp("./push_swap", args);
		perror("Error executing ./push_swap");
		exit(EXIT_FAILURE);
	} else {
		close(pipefd[1]);

		int filefd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (filefd == -1) {
			perror("Error opening file");
			close(pipefd[0]);
			return;
		}

		char buffer[128];
		ssize_t bytesRead;

		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			if (write(filefd, buffer, bytesRead) == -1) {
				perror("Error writing to file");
				close(pipefd[0]);
				close(filefd);
				return;
			}
		}

		close(pipefd[0]);
		close(filefd);

		wait(NULL);
	}
}

// Function to execute the checker program
int exec_checker(char **args) {
	int pipefd[2];
	pid_t pid;

	if (pipe(pipefd) == -1) {
		perror("Error creating pipe");
		return -1;
	}

	pid = fork();
	if (pid == -1) {
		perror("Error creating process");
		return -1;
	}

	if (pid == 0) {
		close(pipefd[0]);

		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		int filefd = open("output.txt", O_RDONLY);
		if (filefd == -1) {
			perror("Error opening output.txt");
			exit(EXIT_FAILURE);
		}
		dup2(filefd, STDIN_FILENO);
		close(filefd);

		execvp("./checker", args);
		perror("Error executing ./checker");
		exit(EXIT_FAILURE);
	} else {
		close(pipefd[1]);

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
				exit(EXIT_FAILURE);
				return -1;
			}
		}

		close(pipefd[0]);
		return -1;
	}
	return -1;
}

// Function to count the number of lines in a file
unsigned int count_lines(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file");
		return -1;
	}

	unsigned int lines = 0;
	int c;

	while ((c = fgetc(file)) != EOF) {
		if (c == '\n') {
			lines++;
		}
	}

	fclose(file);
	return lines;
}

// Function to parse an integer from a string argument
int parse_int(const char *arg) {
	char *endptr;
	int value = strtol(arg, &endptr, 10);
	if (*endptr != '\0') {
		fprintf(stderr, "Error : '%s' is not a valable integer.\n", arg);
		exit(EXIT_FAILURE);
	}
	return value;
}

// Function to print the help message
void print_help() {
	printf("Usage: ./program [options]\n");
	printf("Options :\n");
	printf("  -n <tests>     Number of tests per array size (default: 10, -1 to infinite)\n");
	printf("  -s <start>     Minimum array size (default: 1)\n");
	printf("  -e <end>       Maximum array size (default: 500)\n");
	printf("  -st <steps>    Testing steps (default: 1)\n");
	printf("  -l <limit>     Limit of operations to show a warning (default: 5500)\n");
	printf("  -h             Display this help message\n");
	exit(0);
}

// Main function
int main(int argc, char **argv) {
	unsigned int num_tests = 10;
	int start = 1;
	int end = 500;
	unsigned int steps = 1;
	unsigned int limit = 5500;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
			num_tests = (unsigned int)parse_int(argv[++i]);
		} else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
			start = parse_int(argv[++i]);
		} else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
			end = parse_int(argv[++i]);
		} else if (strcmp(argv[i], "-st") == 0 && i + 1 < argc) {
			steps = (unsigned int)parse_int(argv[++i]);
		} else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
			limit = (unsigned int)parse_int(argv[++i]);
		} else if (strcmp(argv[i], "-h") == 0) {
			print_help();
		} else {
			fprintf(stderr, "Unknown option : %s\n", argv[i]);
			print_help();
		}
	}

	if (start > end) {
		fprintf(stderr, "Error : min size (-s) should be <= of max size (-e).\n");
		return EXIT_FAILURE;
	}

	int *array;
	for (int i = start; i <= end; i += steps) {
		printf("Testing with " PURPLE "%d" RESET " elements:\n", i);
		int min = -1, max = -1;

		for (unsigned int j = 0; j < num_tests; j++) {
			array = generate_random_array(i);
			if (!array)
				return EXIT_FAILURE;

			char *args[1002] = {"./push_swap", NULL};
			add_int_tab_args(args, array, i);
			exec_push_swap(args);

			unsigned int nb_lines = count_lines("output.txt");
			if (min == -1 || nb_lines < (unsigned int)min)
				min = nb_lines;
			if (max == -1 || nb_lines > (unsigned int)max)
				max = nb_lines;

			args[0] = "./checker";
			switch (exec_checker(args)) {
				case -1:
					write(1, RED "[Error] " RESET, 18);
					break;
				case 0:
					if (nb_lines > limit)
						write(1, YELLOW, 6);
					else
						write(1, GREEN, 6);
					write(1, "[OK] " RESET, 10);
					break;
				case 1:
					write(1, RED "[KO] " RESET, 15);
					break;
				default:
					break;
			}

			int k = 1;
			while (args[k]) {
				free(args[k]);
				k++;
			}

			free(array);
		}
		printf("\nMin: " GREEN "%d" RESET ", Max: " YELLOW "%d" RESET "\n\n", min, max);
	}

	if (remove("output.txt"))
		perror("Error deleting the file");

	return 0;
}
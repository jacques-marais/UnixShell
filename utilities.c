#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "definitions.h"

void checkForRedirections(char *argv[])
{
	int fd;

	// Check each item from the command line for < or >
	for (int i = 0; argv[i] != '\0'; ++i)
	{
		// If argument is <
		if (strcmp(argv[i], "<") == 0) {

			// If the next argument exists, close stdout and open the new output file
			if (argv[i + 1] != '\0') {

				if ((fd = open(argv[i + 1], O_RDONLY)) < 0) {

					fprintf(stderr, "Input redirect failed. File \"%s\" does not exist.\n", argv[i + 1]);
					exit(0);

				} else {

					close(STDIN_FILENO);
					dup(fd);
					close(fd);
				}

			// If the next argument doesn't exist, we have nothing to redirect to
			} else {

				fprintf(stderr, "Input redirect failed. No file given.\n");
				exit(0);
			}


			int j;

			// get rid of < and its argument
			for (j = i; argv[j+2] != '\0'; ++j) {

				strcpy(argv[j], argv[j+2]);
			}

			argv[j] = '\0';
			i--;

		// If argument is >
		} else if (strcmp(argv[i], ">") == 0) {

			fflush(stdout);

			// If there is an argument after this one, then redirect the output to
			// that filename
			if (argv[i + 1] != '\0') {

				fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);

				close (STDOUT_FILENO);

				dup (fd);

				close (fd);

			// If there is no argument after this one, we can't redirect the output
			} else {

				fprintf(stderr, "Output redirection failed. No file given.\n");
				exit(0);
			}


			int j;

			// get rid of > and its argument
			for (j = i; argv[j+2] != '\0'; ++j) {

				strcpy(argv[j], argv[j+2]);
			}

			argv[j] = '\0';
			i--;
		}
	}
}


int findPipes(char *argv[], int *pipeLocations)
{
	int numPipes = 0;

	// Check each argument. If it's a pipe, increment the pipe
	// count and log the index position
	for (int i = 0; argv[i] != '\0'; ++i) {

		if (strcmp(argv[i], "|") == 0) {

			pipeLocations[numPipes] = i;

			numPipes++;
		}
	}

	return numPipes;
}


int isBackgroundProcess(int *argc, char **argv)
{
	int numChars; /* The number of chars in the last argument */

	/* Find number of chars in last argument. */
	for (numChars = 0; argv[*argc - 1][numChars] != '\0'; ++numChars) { }

	/* If the last argument is only a &, remove the last arg, reduce argc, and return true */
	if (strcmp(argv[*argc - 1], "&") == 0) {

		argv[*argc - 1] = '\0';

		*argc = *argc - 1;

		return TRUE;
	}

	/* If last char of last arg is &, remove last char and return true */
	if (argv[*argc - 1][numChars - 1] == '&') {

		argv[*argc - 1][numChars - 1] = '\0';

		return TRUE;
	}

	return FALSE;
}

void spaceOutMetacharacters(char input[])
{
	int numChars; /* Number of chars in input char array. */

	/* Find number of chars in input char array. */
	for (numChars = 0; input[numChars] != '\0'; ++numChars) { }

	/*
	 * ADD SPACE ON LEFT SIDE IF NEEDED
	 * Start at second char. If char is a pipe and there is no
	 * space immediately before the pipe, add a space.
	 */
	for (int i = 1; i < numChars; ++i) {

		/*
		 * If char is a pipe or an angle bracket, move the items in the array ahead
		 * and then add a space on the left side of the found metacharacter.
		 */
		if ((input[i] == '|' || input[i] == '<' || input[i] == '>') && input[i - 1] != ' ') {

			int j;

			for (j = numChars; j > i; --j) {

				input[j] = input[j - 1];
			}

			input[j] = ' ';

			numChars++;
		}
	}

	/*
	 * ADD SPACE ON RIGHT SIDE IF NEEDED
	 * Start at first char and go until second to last char. If char is a pipe and
	 * there is no space immediately after the pipe, add a space.
	 */
	for (int i = 0; i < numChars - 1; ++i) {

		/*
		 * If char is a pipe or an angle bracket, move the items in the array ahead
		 * and then add a space on the right side of the found metacharacter.
		 */
		if ((input[i] == '|' || input[i] == '<' || input[i] == '>') && input[i + 1] != ' ') {

			int j;

			for (j = numChars; j > i + 1; --j) {

				input[j] = input[j - 1];
			}

			input[j] = ' ';

			numChars++;
		}
	}
}

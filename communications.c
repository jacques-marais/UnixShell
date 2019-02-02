#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "definitions.h"
#include "utilities.h"

char *promptString = "my-shell> ";


void printPrompt()
{
	printf("%s", promptString);
}


int getInput(int *numArgs, char *argv[])
{
	char input[(MAX_ARGS * MAX_ARG_LEN) + 1];
	memset(input, '\0', sizeof(input));

	// Read in user's command from stdin
	fgets(input, MAX_ARGS * MAX_ARG_LEN, stdin);

	// Remove newline character from end of line
	size_t newlineIndex = strlen(input) - 1;
	if (input[newlineIndex] == '\n')
		input[newlineIndex] = '\0';

	// Add spaces before and after the <, >, and | metacharacters if needed.
	spaceOutMetacharacters(input);

	// Parse command line into the argv[] string array
	int numTokens;
	char *str, *token, *saveptr;

	for (numTokens = 0, str = input; ; ++numTokens, str = NULL) {

		if (numTokens >= MAX_ARGS) {

			fprintf(stderr, "Error: too many arguments\n");
			return FALSE;
		}

		token = strtok_r(str, WHITESPACE, &saveptr);

		if (token == NULL)
			break;

		argv[numTokens] = malloc(sizeof(char)*strlen(token));

		strcpy(argv[numTokens], token);
	}

	*numArgs = numTokens;

	return TRUE;
}


int checkLastPipe(int *argc, char *argv[], int numPipes, int *pipeLocations)
{
	/* If location of last pipe is same as location of last item on cmd line, ask for more input */
	if (pipeLocations[numPipes - 1] == (*argc - 1)) {

		int numNewArgs = 0;
		char **argvNew = calloc(MAX_ARGS + 1, sizeof(char*));

		printf(">");

		// Get more input from terminal.
		getInput(&numNewArgs, argvNew);

		// If less than or equal to MAX_ARGS, add the additional arguments to argv
		if (*argc + numNewArgs <= MAX_ARGS) {

			for (int i = 0; i < numNewArgs; ++i) {

				argv[*argc] = malloc(sizeof(char)*strlen(argvNew[i]));

				strcpy(argv[*argc], argvNew[i]);

				*argc = *argc + 1;
			}

			numPipes = findPipes(argv, pipeLocations);

			for (int i = 0; i < numNewArgs; ++i) {

				free(argvNew[i]);
			}

			free(argvNew);

		// If too many arguments, show an error and don't execute the command - just return.
		} else {

			fprintf(stderr, "Error: too many arguments\n");

			for (int i = 0; i < numNewArgs; ++i) {

				free(argvNew[i]);
			}

			free(argvNew);

			return FALSE;
		}
	}

	return TRUE;
}

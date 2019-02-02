#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "definitions.h"
#include "communications.h"
#include "utilities.h"
#include "execution.h"

int main (int argc, char *argv[], char *envp[])
{
	for (;;) {

		// Pick up any left over child processes
		if (waitpid(-1, NULL, WNOHANG) > 0)
			printf("---child(ren) finished---\n");

		// Print prompt string to stdout
		printPrompt();

		int argcNew = 0;
		char **argvNew = calloc(MAX_ARGS + 1, sizeof(char*));

		// Get input from terminal. Returns false if too many arguments.
		if (getInput(&argcNew, argvNew) == FALSE)
			continue;

		// If input was empty, restart loop to show command prompt again.
		if (argcNew == 0)
			continue;

		// Break out of the loop and end program if exit or quit are entered.
		if (strcmp(argvNew[0], "exit") == 0 || strcmp(argvNew[0], "quit") == 0)
			break;

		int pipeLocations[MAX_ARGS];
		memset(pipeLocations, '\0', sizeof(pipeLocations));

		// Get the number of pipes and the index position of each pipe
		int numPipes = findPipes(argvNew, pipeLocations);

		// If there are no pipes, execute normally
		if (numPipes == 0) {

			executeWithoutPipes(argcNew, argvNew, envp);

		// If there are pipes, execute the special way to link the outputs and inputs
		} else {

			executeWithPipes(&argcNew, argvNew, envp, numPipes, pipeLocations);
		}

		for (int i = 0; i < argcNew; ++i) {

			free(argvNew[i]);
		}

		free(argvNew);

	}
}

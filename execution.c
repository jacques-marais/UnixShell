#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "definitions.h"
#include "pathhandling.h"
#include "communications.h"
#include "utilities.h"


void handleCommand(char *argv[], char *envp[])
{
	// To store the path of the command entered on the command line
	char commandPath[MAX_FULL_PATH_LEN];
	memset(commandPath, '\0', sizeof(commandPath));

	// If the command name starts with '/', we assume that the full path was given,
	// so we just check that the file exists, has execute permission, and then execute it.
	if (argv[0][0] == '/') {

		// If file does not exist, print to console
		if (access(argv[0], F_OK) != 0) {

			fprintf(stderr, "%s: command not found\n", argv[0]);
			exit(0);

		// If file exists, but we don't have execute permission, print to console
		} else if (access(argv[0], X_OK) != 0) {

			fprintf(stderr, "%s: permission denied\n", argv[0]);
			exit(0);

		// If file exists and we have execute permission, go ahead with
		// checking for redirection and doing execution
		} else {

			execve(argv[0], argv, envp);
		}

	// If command name starts with '.', we need to prepend either the path of the
	// current dir or the parent dir.
	} else if (argv[0][0] == '.') {

		// Prepend the command with either the current or parent dir
		fillWorkingDirs(argv);

		//If file does not exist, print to console
		if (access(argv[0], F_OK) != 0) {

			fprintf(stderr, "%s: command not found\n", argv[0]);
			exit(0);

		//If file exists, but we won't have execute permission, print to console
		} else if (access(argv[0], X_OK) != 0) {

			fprintf(stderr, "%s: permission denied\n", argv[0]);
			exit(0);

		//If file exists and we have execute permission, go ahead with
		// checking for redirection and doing execution
		} else {

			execve(argv[0], argv, envp);
		}

	// If command name did not start with '/' or '.', we assume that we have to
	// search the PATH environment variable for the executable.
	} else {

		// Search PATH and get full path of the given command if it exists
		getPath(argv[0], commandPath);

		// If path is empty string, this means the command was not found in
		// any of the PATH directories, so do nothing.
		if (strcmp(commandPath, "") == 0) {

			exit(0);

			// Do nothing. The getPath function above tells the user what went wrong.

		// If path was found, go ahead with checking for redirection and doing execution
		} else {

			argv[0] = commandPath;

			execve(argv[0], argv, envp);
		}
	}
}


void executeWithoutPipes(int argc, char *argv[], char *envp[])
{
	// Check whether or not & is present and we have to run
	// this in the background. If true, set background int
	// to TRUE and get rid of the &
	int background = isBackgroundProcess(&argc, argv);

	pid_t pid = fork();

	// If child, handle the command (dealing with . or .. or / or path-lookup,
	// redirect input/output if necessary, and call execve
	if (pid == 0) {

		checkForRedirections(argv);
		handleCommand(argv, envp);

	// If parent, wait for child to finish
	} else {

		if (background == FALSE)
			wait(NULL);
	}
}


void executeWithPipes(int *argc, char *argv[], char *envp[], int numPipes, int *pipeLocations)
{
	/*
	 * If there is no command after the last pipe, this function will ask for additional input.
	 * If it returns FALSE, that means too many arguments have been entered, so don't go ahead
	 * with execution.
	 */
	if (checkLastPipe(argc, argv, numPipes, pipeLocations) == FALSE) {

		return;
	}

	// Assume we will run this in the foreground.
	// Checking for & is done later in the code.
	int background = FALSE;

	int pipes[numPipes * 2];

	// Set up the pipes for connecting input/outputs.
	for (int i = 0; i < numPipes; ++i) {

		pipe(pipes + (2 * i));
	}

	// Go through each command given, isolate its arguments, set up the pipes,
	// handle input/output file redirection, and call execve.
	for (int commandNumber = 0; commandNumber < numPipes + 1; ++commandNumber) {

		int commandIndex = 0;

		// If this is not the first command, get the index of the command.
		if (commandNumber > 0) {

			// The command will be located right after the pipe character
			commandIndex = pipeLocations[commandNumber - 1] + 1;
		}

		// Stop index - for helping to isolate the arguments for the current command
		int stopIndex = MAX_ARGS;

		// If this is not the last command, get the index of the next pipe.
		if (commandNumber < numPipes) {

			stopIndex = pipeLocations[commandNumber];
		}

		// Allocate memory for this command's arguments
		//char **argvForCommand = malloc(argc * sizeof(char*));
		char **argvForCommand = calloc(*argc, sizeof(char*));

		int numArgs = 0;

		// Isolate the arguments pertaining to this command.
		// The command is the first argument, and the last
		// arg is right before the next pipe (stop index).
		for (int i = commandIndex; i < stopIndex && argv[i] != '\0'; ++i) {

			size_t size = strlen(argv[i]) + 1;

			argvForCommand[numArgs] = malloc(size * sizeof(char));

			strcpy(argvForCommand[numArgs], argv[i]);

			numArgs++;
		}

		fflush(stdout);

		// If this is the last command and & was included, set background int
		// to TRUE and get rid of the &
		if (commandNumber == numPipes && isBackgroundProcess(&numArgs, argvForCommand) == TRUE) {

			background = TRUE;
		}


		pid_t pid = fork();

		// If child...
		if (pid == 0) {

			// If not last command, redirect output to pipe
			if (commandNumber != numPipes) {

				close(STDOUT_FILENO);

				dup(pipes[(commandNumber * 2) + 1]);
			}

			// If not first command, redirect input so that it comes from pipe
			if (commandNumber != 0) {

				close(STDIN_FILENO);

				dup(pipes[(commandNumber - 1) * 2]);
			}

			// Close all the pipes
			for (int i = 0; i < numPipes * 2; ++i) {

				close(pipes[i]);
			}

			// Handle < and > redirections.
			checkForRedirections(argvForCommand);

			// Handle . and .. and / and path-lookup, then handle
			// input/output file redirection, and then call execve.
			handleCommand(argvForCommand, envp);


		// If parent...
		} else {

			// Free up the memory allocated for the args
			for (int i = 0; i < numArgs; ++i) {

				free(argvForCommand[i]);
			}

			free(argvForCommand);
		}
	}

	// Parent continued...

	// Close all pipes
	for (int i = 0; i < numPipes * 2; ++i) {

		close(pipes[i]);
	}

	// If this is a foreground process, wait for all commands to finish.
	// If this is a background process, just move on.
	if (background == FALSE) {

		for (int commandNumber = 0; commandNumber < numPipes + 1; ++commandNumber) {

			wait(NULL);
		}
	}
}

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "definitions.h"

void getPath(char command[], char path[])
{
	// Get PATH environment variable
	const char *pathVariable = getenv("PATH");

	// Make a copy of PATH
	char *pathVariableCopy = strdup(pathVariable);

	// To store all the directories in the path
	char directoryList[MAX_PATHS][MAX_PATH_LEN];
	memset(directoryList, '\0', sizeof(directoryList));

	int i;
	char *str, *token, *saveptr;

	// Tokenize the PATH variable
	// This isolates the directories in PATH and stores each one into the directoryList
	for (i = 0, str = pathVariableCopy; ; ++i, str = NULL) {

		token = strtok_r(str, ":", &saveptr);

		if (token == NULL)
			break;

		strcpy(directoryList[i], token);
	}

	// Done with this allocated memory
	free(pathVariableCopy);

	// check each directory of the PATH variable and check whether the command exists there.
	for (int pathNumber = 0; pathNumber < i - 1; ++pathNumber) { //added -1

		// For storing the absolute path of the directory being checked, plus a slash, plus the command name
		char fullPath[MAX_PATH_LEN + strlen(command) + 1];
		memset(fullPath, '\0', sizeof(fullPath));

		// Put the path together.
		strcpy(fullPath, directoryList[pathNumber]);
		strcat(fullPath, "/");
		strcat(fullPath, command);

		// Check whether the file exists and store the code created by access()
		int code = access(fullPath, F_OK);

		// If file exists, check that we have execute permission
		if (code == 0) {

			// Check whether we have execute permission for this file
			int code2 = access(fullPath, X_OK);

			// If we have execute permission, copy the full path to the "output" array and we're done.
			if (code2 == 0) {

				strcpy(path, fullPath);

				break;

			// If file exists, but we do not have execute permission, display a message and set "output" array to be empty
			} else {

				fprintf(stderr, "%s: permission denied\n", command);

				strcpy(path, "");

				break;
			}

		// If file does not exist and this is the last path in the directoryList, display a message and set "output" array to be empty
		} else {

			if (pathNumber == i - 2) {

				fprintf(stderr, "%s: command not found\n", command);
				strcpy(path, "");
			}
		}
	}

}


void fillWorkingDirs(char *argv[])
{
	// If there are two periods at the beginning, we need to prepend the parent dir
	if (argv[0][0] == '.' && argv[0][1] == '.') {

		char pathWithoutDots[MAX_ARG_LEN];
		memset(pathWithoutDots, '\0', sizeof(pathWithoutDots));

		int srcIndex, destIndex;

		// Store the path without the dots
		for (srcIndex = 2, destIndex = 0; argv[0][srcIndex] != '\0'; ++srcIndex, ++destIndex) {

			pathWithoutDots[destIndex] = argv[0][srcIndex];
		}

		char currentDir[MAX_FULL_PATH_LEN];
		memset(currentDir, '\0', sizeof(currentDir));

		char parentDir[MAX_FULL_PATH_LEN];
		memset(parentDir, '\0', sizeof(parentDir));

		// Get the current working directory
		getcwd(currentDir, sizeof(currentDir));

		// Get a pointer to the last slash in the path
		// I will use this to isolate the parent directory path
		char *ptrToLastSlash = strrchr(currentDir, '/');

		// Store the parent directory
		for (int i = 0; currentDir+i != ptrToLastSlash; ++i) {

			parentDir[i] = currentDir[i];
		}

		// Put the path together
		strcpy(argv[0], parentDir);
		strcat(argv[0], pathWithoutDots);

	// If there is one period at the beginning, then we will prepend the current working directory path
	} else {

		char pathWithoutDot[MAX_ARG_LEN];
		memset(pathWithoutDot, '\0', sizeof(pathWithoutDot));

		int srcIndex, destIndex;

		// Store the path without the dot
		for (srcIndex = 1, destIndex = 0; argv[0][srcIndex] != '\0'; ++srcIndex, ++destIndex) {

			pathWithoutDot[destIndex] = argv[0][srcIndex];
		}

		char currentDir[MAX_FULL_PATH_LEN];
		memset(currentDir, '\0', sizeof(currentDir));

		// Get the current working directory
		getcwd(currentDir, sizeof(currentDir));

		// Put the path together
		strcpy(argv[0], currentDir);
		strcat(argv[0], pathWithoutDot);
	}
}

#ifndef UTILITIES_H_
#define UTILITIES_H_

/*
 * Check for (and handle) input/output redirection ('<' and '>').
 */
extern void checkForRedirections(char *argv[]);

/*
 * Count the number of pipes on the command line and log the location
 * of each pipe.
 * @return The number of pipes.
 */
extern int findPipes(char *argv[], int *pipeLocations);

/*
 * Check whether or not the last argument is a &.
 * @return TRUE if background process. FALSE if not.
 */
extern int isBackgroundProcess(int *argc, char **argv);

/*
 * For each <, >, and | metacharacter, ensure that there is a space
 * to the left and to the right of it.
 */
extern void spaceOutMetacharacters(char input[]);

#endif

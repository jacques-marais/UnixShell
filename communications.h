#ifndef COMMUNICATIONS_H_
#define COMMUNICATIONS_H_

/*
 * Print the prompt string to the console.
 */
extern void printPrompt();

/*
 * Get input from terminal.
 * @return FALSE if too many arguments. TRUE otherwise.
 */
extern int getInput(int *numArgs, char *argv[]);

/*
 * If there is no command after the last pipe, this function will ask
 * for more input on the console and then add the new input to argv.
 * @return FALSE if there are too many arguments in total. TRUE otherwise.
 */
extern int checkLastPipe(int *argc, char *argv[], int numPipes, int *pipeLocations);

#endif

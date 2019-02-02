#ifndef EXECUTION_H_
#define EXECUTION_H_

/*
 * Given a command (the first word on the command line), this function will
 * check for file existence/permission and execute it. It may make use of
 * the functions fillWorkingDirs and getPath to handle it before executing.
 * It checks for input/output redirection before executing.
 */
extern void handleCommand(char *argv[], char *envp[]);

/*
 * Given a command without pipes, execute in the normal way - checking
 * for background execution and input/output redirection along the way.
 */
extern void executeWithoutPipes(int argc, char *argv[], char *envp[]);

/*
 * Given two or more commands separated by pipes, this function
 * connects the outputs and inputs along the pipline and then
 * executes the commands.
 */
extern void executeWithPipes(int *argc, char *argv[], char *envp[], int numPipes, int *pipeLocations);

#endif

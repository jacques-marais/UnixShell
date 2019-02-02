#ifndef PATHHANDLING_H_
#define PATHHANDLING_H_

/*
 * Given a command, this function searches through the PATH directories to
 * determine whether the command exists and whether we have execute permission.
 */
extern void getPath(char *command, char path[]);

/*
 * Given a command that starts with either "." or "..", this function will get
 * the current working directory and use that to prepend either the current
 * working directory or the parent directory to the command in place of the dots.
 */
extern void fillWorkingDirs(char *argv[]);

#endif

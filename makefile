#
# Default rule to execute when the make command has no arguments
#
all: myshell

#
# utilities: Preprocessor + compiler + assembler step which produces object files.
#
utilities.o: utilities.c utilities.h definitions.h
	gcc -c utilities.c -o utilities.o -Wall -pedantic

#
# communications: Preprocessor + compiler + assembler step which produces object files.
#
communications.o: communications.c communications.h utilities.h definitions.h
	gcc -c communications.c -o communications.o -Wall -pedantic

#
# pathhandling: Preprocessor + compiler + assembler step which produces object files.
#
pathhandling.o: pathhandling.c pathhandling.h definitions.h
	gcc -c pathhandling.c -o pathhandling.o -Wall -pedantic

#
# execution: Preprocessor + compiler + assembler step which produces object files.
#
execution.o: execution.c execution.h pathhandling.h utilities.h definitions.h
	gcc -c execution.c -o execution.o -Wall -pedantic

#
# myshell: Preprocessor + compiler + assembler step which produces object files.
#
myshell.o: myshell.c utilities.h communications.h execution.h definitions.h
	gcc -c myshell.c -o myshell.o -Wall -pedantic

#
# executable: Linker step which produces the final executable file.
#
myshell: myshell.o utilities.o communications.o pathhandling.o execution.o
	gcc myshell.o utilities.o communications.o pathhandling.o execution.o -o myshell -Wall -pedantic

#
# The command-line 'make clean' command executes the following
# command, which removes all files created during the build.
#
clean:
	rm -f myshell myshell.o utilities.o communications.o pathhandling.o execution.o

#
# Lists the "phony" rules in this file ('all' and 'clean')
#
.PHONY: all clean

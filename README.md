# UnixShell
A shell that can execute commands (in foreground or background), can pipe commands, and can redirect the input and/or output.



HOW TO USE:
1. clone or download the directory
2. run "make" in the same directory as the project files
3. run "./myshell" to start the shell
    (try NESTING some shells by calling "./myshell" once you're in an existing "./shell" instance



WHAT IF IT DOESN'T WORK?
- Run through the steps above once more.
- Look at the makefile for potential ways in which you can adapt it to your own system


INSTRUCTIONS:

It seems that all combinations of commands, input/output redirection, piping, and background processes work as they should.

Some examples of commands that work:

ls
ls | wc
ls|wc

ls -l | grep myshell | wc
ls -l | grep myshell | wc &

ls -l|grep myshell|wc
ls -l|grep myshell|wc&

ls -l | grep myshell | wc >output.txt
ls -l | grep myshell | wc>output.txt&

>output.txt ls
ls >output.txt -l

<input.txt wc -l
wc -l <input.txt

wc <input.txt >output.txt

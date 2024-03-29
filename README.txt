Russell Feathers
HW Due Date: 11/01/21
CS344: Operating Systems
HW3--SmallSSH

---------How to Run?------------
make
./smallsh

---------File Descriptions------
Main.c 
int main(void);  main function prompts user for input, and then filters
userinput into Usercommand struct (see info below). After filtering main calls either a preset command function
for exit, cd, and status or calls ExecComm to fork off a new child. 

Prior to every iteration main checks whether any child processes created by ExecComm have terminated
and if so displays that prior to prompting the user.

--------
DollarSign.c
char* dollarSign(char *inputString); dollarSign function takes an input string and replaces all 
double $$ entries with the processID and returns an updated string pointer

--------
Exit.c
void exitPreSet(struct bgProcess *head); Exit function calls kill function to kill all
processes in the bgProcess struct, and frees each struct from memory.  Exit returns and 
main kills the program 

--------
ChangeDirectory.c
void cdPreSet(char** args, int argCounter); changeDirectory function takes arguments
and argumentCounter and determines if input is valid i.e. directory is valid, only 1 
argument. Then function uses chdir to change the working directory if valid. 

--------
Status.c
void statusPreSet(bool firstProcess, int exitStatus, int lastTerminate); returns either the status 
or last terminating signal depending on the last command. if no foreground process (other than preset 
commans) has been run it returns the exit status. 

--------
ExecComm.c
int execComm(struct bgProcess* bgProcessHead, struct userCommand* currCommand, int* exitStatus)
function forks off a new child process and redirects input and output based on command input parameters.
If process is a background process and no input and output path is specified then path is updated
to "/dev/null".  For foreground processes execComm will wait for the child process to commence before
returning to the command line.  For background processes execComm will add bgProcess to bgProces linked list
and display the process id of the new background process before returning to the command line.

--------
Redirection.c
int inputRedirection(char* newInput, int oldInput, int* exitStatus)
function redirects input using dup2 to be as specified from ExecComm from standard input to 
specified location.  function does error checking and returns -1 if redirection is not possible 
and displays to terminal specific error messages.

int outputRedirection(char* newOutput, int oldOutput, int* exitStatus)
function redirects output using dup2 to be as specified from ExecComm from standard output to 
specified location.  function does error checking and returns -1 if redirection is not possible 
and displays to terminal specific error messages.

--------
CheckBgTerm.c
void checkBgTerm(struct bgProcess* bgProcessHead, int* exitStatus); function checks whether any background
processes have terminated by iterating through linked list.  If any background processes have terminated then
CheckBgTerm prints to terminal the name of the process and the id.  Bg process frees memory from each item of linked
list when the item has finished. 

--------
SignalShell.c
void handle_SIGTSTP_shell(int signo); function creates handler for shell that enables/ disables foreground mode after a SIGTSTP signal

void signalShell(void); function sets the actions for the SIGINT and SIGTSTP signals.  Shell ignores SIGINT and "handles" SIGTSTP

void signal_SIGINT_fg_update(void); function sets foreground commands response to signals.  For SIGINT to be the default action 
(i.e. kill process) and to ignore SIGTSTP.

void signal_SIGINT_bg_update(void); function sets background commands response to signals.  For SIGINT and SIGTSTP the commands will 
ignore the signals.

---------
UserCommand.h
struct: userCommand; contains char* command, char** args, char* fileinput,
char* fileoutput, _Bool fg;

---------
BGProcesses.h
struct: bgProcess; contains processID, parent processID, and the next item in the linked list of processes. bgProcess holds
all the running background processes. 
Russell Feathers
HW Due Date: 11/01/21
HW3--SmallSSH

---------How to Run?------------
make
./smallsh

---------File Descriptions------
Main.c 
int main(void);  main function prompts user for input, and then filters
userinput into Usercommand struct (see info below). After filtering main calls either a preset command function
for exit, cd, and status or calls ExecComm to fork off a new child. 

--------
DollarSign.c
char* dollarSign(char *inputString); dollarSign function takes an input string and replaces all 
double $$ entries with the processID and returns an updated string pointer

--------
Exit.c
void exitPreSet(struct bgProcess *head); Exit function calls kill function to kill all
processes in the bgProcess struct.  Exit returns and main kills the program 

--------
ChangeDirectory.c
void cdPreSet(char** args, int argCounter); changeDirectory function takes arguments
and argumentCounter and determines if input is valid i.e. directory is valid, only 1 
argument. Then function uses chdir to change the working directory if valid. 

--------
Status.c
void statusPreSet(bool firstProcess, int exitStatus, int lastTerminate); returns the status
of the program if no foreground process (other than preset commans) has been run and returns
the last signal number of the terminating signal for a foreground process otherwise

--------
ExecComm.c
function:

---------
UserCommand.h
struct: userCommand; which contains char* command, char** args, char* fileinput,
char* fileoutput, _Bool fg;

---------
BGProcesses.h
struct bgProcess contains processID, parent processID, and the next item in the linked list of processes. bgProcess holds
all the running background processes. 
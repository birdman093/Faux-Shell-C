#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include "UserCommand.h"
#include "BGProcesses.h"

#define maxinput 2049
#define hashtag '#'
#define input '<'
#define output '>'
#define background '&'
#define EXITPRESET "exit"
#define CDPRESET "cd"
#define STATUSPRESET "status"
#define MAXARGS 514   // accounts for max 512 arguments, 1 command, 1 NULL termination

char* dollarSign(char *);
void exitPreSet(struct bgProcess*);
void cdPreSet(char**, int);
void statusPreSet(bool, int, int);
int execComm(struct bgProcess*, struct userCommand*, int*);

int main(void) {

    //initialize exitStatus, userInput, and command struct pointer
    int exitStatus = 0;
    int lastTerminate = -1;
    struct bgProcess* bgProcessHead = NULL; //malloc(sizeof(struct bgProcess));
    bool firstProcess = false;  // tracks whether first foreground process other than built-ins has run yet
    

    while(1) {
        //Prompt User for command
        char* userInput = calloc(maxinput, sizeof(char));
        printf("\n: ");
        fgets(userInput, maxinput, stdin);

        //Break first input into tokens, check for valid input (i.e. no input, or # comment character)
        char* token = strtok_r(userInput," \n\r",&userInput);
        if(token == NULL) {
            continue;
        }
        char firstChar = token[0];
        if (hashtag == firstChar) {
            continue;
        }
        // allocate and initialize command for userCommand struct, modify command based on $$ expansion
        struct userCommand* currCommand = malloc(sizeof(struct userCommand));  ;
        char* modToken = dollarSign(token);
        currCommand->command = calloc(strlen(modToken)+1, sizeof(char));
        strcpy(currCommand->command, modToken);
        
        // initialize first argument as command
        currCommand->args = calloc(MAXARGS, sizeof(char*));
        currCommand->args[0] = calloc(strlen(modToken)+1, sizeof(char));
        strcpy(currCommand->args[0], modToken);

        // initialize rest of command
        currCommand->argcounter = 1;
        currCommand->fileinput = NULL;
        currCommand->fileoutput = NULL;
        currCommand->fg = true;

        // iterate through each item based on spaces, and place into command struct
        int argCounter = 1;
        token = strtok_r(NULL," \n\r",&userInput);
        while (token != NULL) {
            modToken = dollarSign(token);
            switch(modToken[0]) {
                case input :
                    currCommand->fileinput = calloc(strlen(modToken)+1, sizeof(char));
                    modToken++;
                    strcpy(currCommand->fileinput, modToken);
                    break;
                case output :
                    currCommand->fileoutput = calloc(strlen(modToken)+1, sizeof(char));
                    modToken++;
                    strcpy(currCommand->fileoutput, modToken);
                    break;
                case background :
                    currCommand->fg = false;
                    break;
                default :
                    currCommand->args[argCounter] = calloc(strlen(modToken)+1, sizeof(char));
                    strcpy(currCommand->args[argCounter], modToken);
                    currCommand->argcounter ++;
                    argCounter++;
            }

            // background is last possible symbol, so stop command at this point
            if (currCommand->fg == false) {
                break;
            }
            token = strtok_r(NULL,"  \n\r",&userInput);
        }
        currCommand->args[argCounter] = NULL;

        // for testing purposes only
        printf("Command: %s \nfileInput: %s \nfileOutput: %s \nbackground: %d\n", currCommand->command, currCommand->fileoutput, currCommand->fileinput, currCommand->fg);
        int counter = 0;
        while(currCommand->args[counter] != NULL) {
            printf("arg: %s\n", currCommand->args[counter]);
            counter++;
        }

        //Direct Pre-Set and Non Pre-Set Commands to functions
        if (strcmp(currCommand->command, EXITPRESET) == 0) {    
            exitPreSet(bgProcessHead);
            free(bgProcessHead);
            free(currCommand);
            exit(exitStatus);
        } else if (strcmp(currCommand->command, CDPRESET) == 0) {
            cdPreSet(currCommand->args, currCommand->argcounter);
        } else if (strcmp(currCommand->command, STATUSPRESET) == 0) {
            statusPreSet(firstProcess, exitStatus, lastTerminate);
        } else {
            // create new process in exec, and check if this is first foreground process
            int fgCheck = execComm(bgProcessHead, currCommand, &exitStatus);
            if (firstProcess == false && currCommand->fg && fgCheck != -1) {
                firstProcess = true;
            }
        }
        
        // Q: do we need to free each item in struct???
        free(currCommand); 
    }
}
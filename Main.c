#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include "UserCommand.h"

#define maxinput 2049
#define hashtag '#'
#define input '<'
#define output '>'
#define background '&'

char* dollarSign(char *);

int main(void) {

    //int bgProcesses[10];
    int status = 0;
    char* userInput = calloc(2049, sizeof(char));
    struct userCommand* currCommand;

    while(1) {
        //Prompt User for command
        
        printf(": ");
        fgets(userInput, maxinput, stdin);

        //If no user input, reprompt user
        if(strlen(userInput) == 0) {
            continue;
        }

        //Break first input into tokens
        char* token = strtok_r(userInput," ",&userInput);

        // Check if command is a comment
        char firstChar = token[0];
        if (hashtag == firstChar) {
            continue;
        }

        currCommand = malloc(sizeof(struct userCommand));  
        char* modToken = dollarSign(token);
        currCommand->command = calloc(strlen(modToken), sizeof(char));

        token = strtok_r(NULL," ",&userInput);
        currCommand->args = calloc(512, sizeof(char*));
        int argCounter = 0;
        while (token != NULL) {
            switch(token[0]) {
                case input :
                    currCommand->fileinput = calloc(strlen(token), sizeof(char));
                    break;
                case output :
                    currCommand->fileoutput = calloc(strlen(token), sizeof(char));
                    break;
                case background :
                    currCommand->fg = false;
                    break;
                default :
                    currCommand->args[argCounter] = calloc(strlen(token), sizeof(char));
                    argCounter ++;
            }
            token = strtok_r(NULL," ",&userInput);
        }
        free(currCommand); 
    }
    free(userInput);
}
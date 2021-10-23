#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

#define dollar '$'

// dollarSign function takes an input string and replaces all double $$ entries with the processID and 
// returns an updated string pointer
char* dollarSign(char *inputString) {
    
    //search inputString for Number of $$ expressions that need to be modified
    pid_t processID = getpid();
    _Bool match1 = false;
    int doubleMatch = 0;
    int strPtrInp = 0;

    while (inputString[strPtrInp] != '\0') {
        if (inputString[strPtrInp] == dollar) {
            if (match1 == false) {
                match1 = true;
            } else {
                doubleMatch ++;
                match1 = false;
            }
        } else {
            match1 = false;
        }
        strPtrInp ++;
    }

    //Creates new String based on number of $$ expressions
    if (doubleMatch > 0) {
        char* processString = calloc(11,sizeof(char));  // max value of integer is 10 characters + null terminator
        sprintf(processString, "%d", processID);
        char* modString = calloc(strlen(inputString) + doubleMatch*(strlen(processString)-2)+1,sizeof(char));
        strPtrInp = 0;

        while (inputString[strPtrInp] != '\0') {
            if (inputString[strPtrInp] == dollar && inputString[strPtrInp + 1] == dollar) {
                    strcat(modString, processString);
                    strPtrInp ++;
            } else {
                strncat(modString, &inputString[strPtrInp],1);
            }
            strPtrInp ++;
        }
        return modString;
    } else {
        return inputString;
    }
}
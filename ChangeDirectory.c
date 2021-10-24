#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include "UserCommand.h"

#define MAX_FILE_LENGTH 256

void cdPreSet(char** args, int argCounter) {
    char* buffer = malloc(MAX_FILE_LENGTH*sizeof(char));
    int directChange;

    //check argument length is exactly one
    if (argCounter > 2) {
        printf("Error on Input: Too many arguments");
        return;
    }

    // get current working directory and updates directory based on argument 0 or updates directory to 
    // HOME if no arguments.  Checks for errors in processes and outputs to terminal if error.
    char* currDir = getcwd(buffer, MAX_FILE_LENGTH);
    if (currDir == NULL) {
        printf("Error getting current working directory");
        return;
    } else if (argCounter == 0) {
        char* homeDir = getenv("HOME");
        directChange = chdir(homeDir);
        if (directChange == -1) {
            printf("%s: Error updating to HOME directory", homeDir);
        } 
    } else {
        directChange = chdir(args[1]);
        if (directChange == -1) {
            printf("%s: No such file or directory", args[1]);
        }
    }
}
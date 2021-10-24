#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "UserCommand.h"
#include "BGProcesses.h"

#define INV -5

int inputRedirection(char*, int, int*);
int outputRedirection(char*, int, int*);

int execComm(struct bgProcess* bgProcessHead, struct userCommand* currCommand, int* exitStatus) {
    pid_t newProcess = -5;
    int childStatus;
    newProcess = fork();
    if (newProcess == -1) {
        // Error in process creation
        printf("Invalid Fork--New Process did not fork\n");
        return -1;
    } else if (newProcess == 0) {
        // Child process

        // Validate command before input and output redirection
        if (strcmp(currCommand->command, "yada")==0) {
            exit(EXIT_FAILURE);
        }

        // Input and Output redirection based on command, returns -1 on bad result
        int fd_OutResult = 0;
        int fd_InResult = 0;
        if (currCommand->fileoutput != NULL) {
            fd_OutResult = outputRedirection(currCommand->fileoutput, 1, exitStatus);
        }
        if (currCommand->fileinput != NULL) {
            fd_InResult = inputRedirection(currCommand->fileinput, 0, exitStatus);
        }
        if (fd_OutResult == -1 || fd_InResult == -1) {
            exit(EXIT_FAILURE);
        }

        // Search PATH environment to find command, exit based on status of search
        int execResult = execvp(currCommand->args[0],currCommand->args);
        if (execResult == -1) {
            if (currCommand->fileoutput != NULL) {
                close(fd_OutResult);
            }
            if (currCommand->fileinput != NULL) {
                close(fd_InResult);
            }
            exit(EXIT_FAILURE);
        } else {
            exit(EXIT_SUCCESS); 
        }
        
    } else {
        // Parent process, if foreground wait for child process to finish, 
        // otherwise do not wait
        if (currCommand->fg) {
            pid_t childResponse = waitpid(newProcess, &childStatus, 0);
            // terminated normally-- this may not get used
            if (WIFEXITED(childStatus)) {
                if (WEXITSTATUS(childStatus) == EXIT_FAILURE) {
                    *exitStatus = 1;
                    printf("TESTINGONLY: Invalid Command, %d\n", childResponse);
                    return -1;
                } else {
                    printf("TESTINGONLY: Valid Command, %d\n", childResponse);
                    return 1;
                }
            } else {
                // terminated abnormally
                printf("This terminated abnormally: %d\n", WTERMSIG(childStatus));
                return -1;
            }
            
        } else {
            //background process things
            printf("I AM BACKGROUND");
            return 1;
        }
    }
}
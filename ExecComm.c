#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "UserCommand.h"
#include "BGProcesses.h"

#define INV -5

int execComm(struct bgProcess* bgProcessHead, struct userCommand* currCommand, int* exitStatus) {
    pid_t newProcess = -5;
    int childStatus;
    newProcess = fork();
    if (newProcess == -1) {
        printf("Invalid Fork--New Process did not fork\n");
    } else if (newProcess == 0) {
        // Child process

        //call execvp, which searches PATH environment to find command, if exec
        // is successful return 0 other return INV
        int execResult = execvp(currCommand->args[0],currCommand->args);
        if (execResult == -1) {
            exit(EXIT_FAILURE);
        } else {
            exit(EXIT_SUCCESS); 
        }
        
    } else {
        // Parent process, if foreground wait for child process to finish, 
        // otherwise do not wait
        if (currCommand->fg) {
            pid_t childResponse = waitpid(newProcess, &childStatus, 0);
            pid_t parID = getpid();

            // terminated normally-- this may not get used
            if (WIFEXITED(childStatus)) {
                printf("\nThis terminated normally: %d\n", WEXITSTATUS(childStatus));
                if (WEXITSTATUS(childStatus) == EXIT_FAILURE) {
                    *exitStatus = 1;
                }
            } else {
                // terminated abnormally
                printf("This terminated abnormally: %d\n", WTERMSIG(childStatus));
            }
            
            printf("ChildStatus: %d\n", WEXITSTATUS(childStatus));
            printf("ChildID %d\nParentID: %d\n",childResponse, parID);
        } else {
            //background process things
            printf("I AM BACKGROUND");
        }
    }
    return -1;
}
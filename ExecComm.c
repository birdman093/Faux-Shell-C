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

int execComm(struct bgProcess* bgProcessHead, struct userCommand* currCommand) {
    pid_t newProcess = -5;
    int childStatus;
    newProcess = fork();
    if (newProcess == -1) {
        printf("Invalid Fork--New Process did not fork\n");
    } else if (newProcess == 0) {
        // Child process, call function using exec
        printf("I AM CHILD\n");
        _exit(0); 
    } else {
        // Parent process, if foreground wait for child process to finish, 
        // otherwise do not wait
        if (currCommand->fg) {
            pid_t childResponse = waitpid(newProcess, &childStatus, 0);
            pid_t parID = getpid();
            printf("ChildID %d\nParentID: %d\n",childResponse, parID);
        } else {
            //background process things
            printf("I AM BACKGROUND");
        }
    }
    printf("why here?");
    return -1;
}
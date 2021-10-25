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
#include <signal.h>
#include "UserCommand.h"
#include "BGProcesses.h"

#define INV -5
#define DEFAULT_BG_REDIR "/dev/null"

int inputRedirection(char*, int, int*);
int outputRedirection(char*, int, int*);
void signal_SIGINT_fg_update(void);
void signal_SIGINT_bg_update(void);

int execComm(struct bgProcess** bgProcessHead, struct userCommand* currCommand, int* exitStatus) {
    pid_t newProcess = -5;
    int childStatus;
    newProcess = fork();
    if (newProcess == -1) {
        // Error in process creation
        printf("Invalid Fork--New Process did not fork\n");
        return -1;
    } else if (newProcess == 0) {
        // Child process

        // Input and Output redirection based on command, if process is a background process
        // then it is redirected to /dev/null,  returns -1 on bad result
        int fd_OutResult = 0;
        int fd_InResult = 0;
        if (currCommand->fileoutput != NULL) {
            fd_OutResult = outputRedirection(currCommand->fileoutput, 1, exitStatus);
        } else if (currCommand->fg == false) {
            fd_OutResult = outputRedirection(DEFAULT_BG_REDIR, 1, exitStatus);
        }
        if (currCommand->fileinput != NULL) {
            fd_InResult = inputRedirection(currCommand->fileinput, 0, exitStatus);
        } else if (currCommand->fg == false) {
            fd_InResult = inputRedirection(DEFAULT_BG_REDIR, 0, exitStatus);
        }
        if (fd_OutResult == -1 || fd_InResult == -1) {
            exit(EXIT_FAILURE);
        }

        //assign new SIGINT handler based on fg or bg
        if (currCommand->fg == true) {
            signal_SIGINT_fg_update();
        } else {
            signal_SIGINT_bg_update();
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
                printf("NOT AT HANDLER: This terminated abnormally, killed by Signal %d\n", WTERMSIG(childStatus));
                return -1;
            }
            
        } else {
            // add process to bgProcesses by making it the head and display on printf
            struct bgProcess* currBG = malloc(sizeof(struct bgProcess));
            currBG->processID = newProcess;
            currBG->parProcessID = getpid();
            
            if (*bgProcessHead == NULL) {
                currBG->next = NULL;
            } else {
                struct bgProcess* bgProcessTmp = *bgProcessHead;
                currBG->next = bgProcessTmp;
            }
            *bgProcessHead = currBG;
            printf("Background pid is %d\n", newProcess);

            return 1;
        }
    }
}
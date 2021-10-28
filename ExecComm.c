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
#define EXIT_FAILURE_INPUT 11
#define EXIT_FAILURE_OUTPUT 12
#define EXIT_FAILURE_COMMAND 13

int inputRedirection(char*, int);
int outputRedirection(char*, int);
void signal_SIGINT_fg_update(void);
void signal_SIGINT_bg_update(void);

int execComm(struct bgProcess** bgProcessHead, struct userCommand* currCommand, int* exitStatus, int* lastTerminate) {
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

        if (currCommand->fileinput != NULL) {
            fd_InResult = inputRedirection(currCommand->fileinput, 0);
        } else if (currCommand->fg == false) {
            fd_InResult = inputRedirection(DEFAULT_BG_REDIR, 0);
        }
        if (fd_InResult == -1) {
            exit(EXIT_FAILURE_INPUT);
        }

        if (currCommand->fileoutput != NULL) {
            fd_OutResult = outputRedirection(currCommand->fileoutput, 1);
        } else if (currCommand->fg == false) {
            fd_OutResult = outputRedirection(DEFAULT_BG_REDIR, 1);
        }

        if (fd_OutResult == -1) {
            exit(EXIT_FAILURE_OUTPUT);
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
            exit(EXIT_FAILURE_COMMAND);
        } else {
            exit(EXIT_SUCCESS); 
        }
        
    } else {
        // Parent process, if foreground wait for child process to finish, 
        // otherwise do not wait
        if (currCommand->fg) {
            pid_t childResponse;
            while((childResponse = waitpid(newProcess, &childStatus, 0)) != newProcess) {}

            if (WIFEXITED(childStatus)) {
                
                if (WEXITSTATUS(childStatus) == EXIT_FAILURE_COMMAND) {
                    *exitStatus = 1;
                    printf("%s, is an Invalid File/ Command\n", currCommand->command);
                    return -1;
                } else if (WEXITSTATUS(childStatus) == EXIT_FAILURE_INPUT) {
                    *exitStatus = 1;
                    printf("Error Opening Input File, %s, for Input Redirection", currCommand->fileinput);
                    return -1;
                } else if (WEXITSTATUS(childStatus) == EXIT_FAILURE_OUTPUT) {
                    *exitStatus = 1;
                    printf("Error Opening Output File, %s, for Output Redirection", currCommand->fileinput);
                    return -1;
                } else if (WEXITSTATUS(childStatus) == EXIT_FAILURE) {
                    *exitStatus = 1;
                    return -1;
                }
                else {
                    printf("TESTINGONLY: Valid Command, %d\n", childResponse);
                    *exitStatus = 0;
                    return 1;
                }
            } else {
                *exitStatus = 1;
                *lastTerminate = WTERMSIG(childStatus);
                printf("Terminated by Signal %d\n", WTERMSIG(childStatus));
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
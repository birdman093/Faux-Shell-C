#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "UserCommand.h"
#include "BGProcesses.h"

#define EXIT_FAILURE_INPUT 11
#define EXIT_FAILURE_OUTPUT 12
#define EXIT_FAILURE_COMMAND 13

void checkBgTerm(struct bgProcess** bgProcessHead) {

    // Iterate through bgProcesses linked list and see if any background processes have terminated
    struct bgProcess* prevBgProcess = NULL;
    struct bgProcess* currBgProcess = *bgProcessHead;
    int bgStatus;
    int bgResult;
    int bgExitStatus = 0;
    int bgLastTerminate = -1;

    while(currBgProcess != NULL) {

        bgResult = waitpid(currBgProcess->processID, &bgStatus, WNOHANG);
        // checking time!
        if (bgResult != 0) {
            //update last terminate
            if (WIFEXITED(bgStatus)) {
                if (WEXITSTATUS(bgStatus) == EXIT_FAILURE_COMMAND || WEXITSTATUS(bgStatus) == EXIT_FAILURE_INPUT || WEXITSTATUS(bgStatus) == EXIT_FAILURE_OUTPUT) {
                    bgExitStatus = 1;
                } else {
                    bgExitStatus = 0;
                }
                printf("background pid %d is done: exit value %d\n", currBgProcess->processID, bgExitStatus);
            } else {
                bgLastTerminate = WTERMSIG(bgStatus);
                printf("background pid %d is done: terminated by signal %d\n", currBgProcess->processID, bgLastTerminate);
            }

            if (prevBgProcess == NULL) {
                //head of list needs to be updated
                *bgProcessHead = currBgProcess->next;
                //free(currBGProcess);
                currBgProcess = currBgProcess->next;
            } else {
                //middle/end of list needs to be updated
                prevBgProcess->next = currBgProcess->next;
                //free(currBgProcess);
                currBgProcess = prevBgProcess->next;
            }

        } else {
            prevBgProcess = currBgProcess;
            currBgProcess = currBgProcess->next;
        }
    }
}
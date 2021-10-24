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

void checkBgTerm(struct bgProcess** bgProcessHead, int* exitStatus) {

    // Iterate through bgProcesses linked list and see if any background processes have terminated
    struct bgProcess* prevBgProcess = NULL;
    struct bgProcess* currBgProcess = *bgProcessHead;
    int bgStatus;
    int bgResult;

    while(currBgProcess != NULL) {

        bgResult = waitpid(currBgProcess->processID, &bgStatus, WNOHANG);
        // checking time!
        if (bgResult != 0) {
            printf("background pid %d is done: exit value %d\n", currBgProcess->processID, *exitStatus);
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
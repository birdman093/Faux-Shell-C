#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include "UserCommand.h"
#include "BGProcesses.h"
#include <signal.h>

void exitPreSet(struct bgProcess *head) {
    if (head != NULL) {
        struct bgProcess* currProcess = head;
        struct bgProcess* tmp;
        while(currProcess != NULL) {
            //kill each process with SIGKILL (9)!!! and then free the memory in pgProcess
            if (kill(currProcess->processID, 9) == -1) {
                printf("%d did not close correctly",currProcess->processID);
            }
            tmp = currProcess;
            currProcess = currProcess->next;
            free(tmp);
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include "UserCommand.h"

void statusPreSet(bool firstProcess, int exitStatus, int lastTerminate) {
    if (!firstProcess) {
        printf("Exit Status %d\n", exitStatus);
    } else {
        printf("Terminated by Signal %d\n", lastTerminate);
    }
}
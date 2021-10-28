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

int inputRedirection(char* newInput, int oldInput) {
    int fd_In = open(newInput, O_RDONLY);
    if (fd_In == -1) { return -1; }

    int fd_InResult = dup2(fd_In, oldInput);
    if (fd_InResult == -1) { return -1; }

    return fd_InResult;
}

int outputRedirection(char* newOutput, int oldOutput) {
    int fd_Out = open(newOutput, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_Out == -1) { return -1; }

    int fd_OutResult = dup2(fd_Out, oldOutput);
    if (fd_OutResult == -1) { return -1; }
    
    return fd_OutResult;
}
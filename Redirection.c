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

int inputRedirection(char* newInput, int oldInput, int* exitStatus) {
    int fd_In = open(newInput, O_RDONLY);
    if (fd_In == -1) {
        printf("Error Opening/Creating Output Redirection: %s", newInput);
        *exitStatus = 1;
        return -1;
    }
    int fd_InResult = dup2(fd_In, oldInput);
    if (fd_InResult == -1) {
        printf("Error Redirecting from Standard Out to output file: %s", newInput);
        *exitStatus = 1;
        return -1;
    }
    return fd_InResult;
}

int outputRedirection(char* newOutput, int oldOutput, int* exitStatus) {
    int fd_Out = open(newOutput, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_Out == -1) {
        printf("Error Opening/Creating Output Redirection: %s", newOutput);
        *exitStatus = 1;
        return -1;
    }
    int fd_OutResult = dup2(fd_Out, oldOutput);
    if (fd_OutResult == -1) {
        printf("Error Redirecting from Standard Out to output file: %s", newOutput);
        *exitStatus = 1;
        return -1;
    }
    return fd_OutResult;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

struct bgProcess {

    pid_t processID;
    pid_t parProcessID;
    struct bgProcess *next;
};
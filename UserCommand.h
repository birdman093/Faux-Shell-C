#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

struct userCommand {

    char* command;
    char** args;
    int argcounter;
    char* fileinput;
    char* fileoutput;
    _Bool fg;
};
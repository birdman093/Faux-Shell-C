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
#include <signal.h>
#include "UserCommand.h"
#include "BGProcesses.h"

sigset_t shell_Signal_Set;

void handle_SIGINT_fg(int signo) {
    char* message = "Terminated by Signal 2\n";
    fflush(stdout);
    write(STDOUT_FILENO, message, 23);
    int fd_Out = open("ErrorLog_FG.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    write(fd_Out, message, 23);
    exit(EXIT_FAILURE);
}

void handle_SIGTSTP_shell(int signo) {
    char* message = "Terminated by Signal 20\n";
    int fd_Out = open("ErrorLog_Shell.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    write(fd_Out, message, 24);
}

void signalShell(void) {
    sigemptyset(&shell_Signal_Set);
    sigaddset(&shell_Signal_Set, SIGINT);
    sigaddset(&shell_Signal_Set, SIGTSTP);

    struct sigaction shellSIGINT;
    shellSIGINT.sa_handler = SIG_IGN;
    sigfillset(&shellSIGINT.sa_mask);
    shellSIGINT.sa_flags = 0;

    struct sigaction shellSIGTSTP;
    shellSIGTSTP.sa_handler = handle_SIGTSTP_shell;
    sigfillset(&shellSIGTSTP.sa_mask);
    shellSIGTSTP.sa_flags = 0;

    sigaction(SIGINT, &shellSIGINT, NULL);
    sigaction(SIGTSTP, &shellSIGTSTP, NULL); 
}

void signal_SIGINT_fg_update(void) {
    //sigset_t fg_Signal_Set;
    //int result1 = sigemptyset(&fg_Signal_Set);
    //int result2 = sigaddset(&fg_Signal_Set, SIGINT);
    struct sigaction fgSIGINT;
    fgSIGINT.sa_handler = handle_SIGINT_fg;
    sigfillset(&fgSIGINT.sa_mask);
    fgSIGINT.sa_flags = 0;

    struct sigaction fgSIGTSTP;
    fgSIGTSTP.sa_handler = handle_SIGINT_fg;
    sigfillset(&fgSIGTSTP.sa_mask);
    fgSIGTSTP.sa_flags = 0;

    sigemptyset(&shell_Signal_Set);
    sigaddset(&shell_Signal_Set, SIGINT);
    sigaddset(&shell_Signal_Set, SIGTSTP);
    sigaction(SIGINT, &fgSIGINT, NULL);
    sigaction(SIGTSTP, &fgSIGTSTP, NULL);

}

void signal_SIGINT_bg_update(void) {
    struct sigaction bgSIGTSTP;
    bgSIGTSTP.sa_handler = handle_SIGINT_fg;
    sigfillset(&bgSIGTSTP.sa_mask);
    bgSIGTSTP.sa_flags = 0;

    sigaddset(&shell_Signal_Set, SIGTSTP);
    sigaction(SIGTSTP, &bgSIGTSTP, NULL);

}
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
bool global_Background;
int lastTerminate;

void handle_SIGTSTP_shell(int signo) {
    global_Background = !global_Background;
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
    struct sigaction fgSIGINT;
    fgSIGINT.sa_handler = SIG_DFL;
    sigfillset(&fgSIGINT.sa_mask);
    fgSIGINT.sa_flags = 0;

    struct sigaction fgSIGTSTP;
    fgSIGTSTP.sa_handler = SIG_IGN;
    sigfillset(&fgSIGTSTP.sa_mask);
    fgSIGTSTP.sa_flags = 0;

    int ret1 = sigemptyset(&shell_Signal_Set);
    int ret2 = sigaddset(&shell_Signal_Set, SIGINT);
    int ret3 = sigaddset(&shell_Signal_Set, SIGTSTP);
    int ret4 = sigaction(SIGINT, &fgSIGINT, NULL);
    int ret5 = sigaction(SIGTSTP, &fgSIGTSTP, NULL);

    // for testing only
    if (ret1 == -1 || ret2 == -1 || ret3 == -1 || ret4 == -1 || ret5 == -1) {
        char* message = "Terminated by Signal re\n";
        int fd_Out = open("ErrorLog_FG.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
        write(fd_Out, message, 24);
    }

}

void signal_SIGINT_bg_update(void) {
    struct sigaction bgSIGTSTP;
    bgSIGTSTP.sa_handler = SIG_IGN;
    sigfillset(&bgSIGTSTP.sa_mask);
    bgSIGTSTP.sa_flags = 0;

    sigaddset(&shell_Signal_Set, SIGTSTP);
    sigaction(SIGTSTP, &bgSIGTSTP, NULL);

}
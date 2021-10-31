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
bool global_Background_On;

void handle_SIGTSTP_shell(int signo) {
    // handler for shell signals
    global_Background_On = !global_Background_On;
    if (!global_Background_On) {
        char* message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 48);
    } else {
        char* message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 28);
    }
}

void signalShell(void) {
    // set signals for shell to ignore SIGINT, and handle SIGTSTP
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
    // update actions for foreground processes to default for SIGINT and ignore for SIGTSTP
    struct sigaction fgSIGINT;
    fgSIGINT.sa_handler = SIG_DFL;
    sigfillset(&fgSIGINT.sa_mask);
    fgSIGINT.sa_flags = 0;

    struct sigaction fgSIGTSTP;
    fgSIGTSTP.sa_handler = SIG_IGN;
    sigfillset(&fgSIGTSTP.sa_mask);
    fgSIGTSTP.sa_flags = 0;

    sigemptyset(&shell_Signal_Set);
    sigaddset(&shell_Signal_Set, SIGINT);
    sigaddset(&shell_Signal_Set, SIGTSTP);
    sigaction(SIGINT, &fgSIGINT, NULL);
    sigaction(SIGTSTP, &fgSIGTSTP, NULL);
}

void signal_SIGINT_bg_update(void) {
    // update actions for background processes to ignore SIGTSTP
    struct sigaction bgSIGTSTP;
    bgSIGTSTP.sa_handler = SIG_IGN;
    sigfillset(&bgSIGTSTP.sa_mask);
    bgSIGTSTP.sa_flags = 0;

    sigaddset(&shell_Signal_Set, SIGTSTP);
    sigaction(SIGTSTP, &bgSIGTSTP, NULL);

}
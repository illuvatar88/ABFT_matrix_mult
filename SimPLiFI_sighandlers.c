#include <signal.h>
#include "SimPLiFI_sighandlers.h"
#include <stdio.h>

void SimPLiFI_register_sighandlers() {
    signal(SIGSEGV, SimPLiFI_sighandler_SIGSEGV);
    signal(SIGILL, SimPLiFI_sighandler_SIGILL);
    signal(SIGBUS, SimPLiFI_sighandler_SIGBUS);
    signal(SIGFPE, SimPLiFI_sighandler_SIGFPE);
    signal(SIGABRT, SimPLiFI_sighandler_SIGABRT);
    signal(SIGIOT, SimPLiFI_sighandler_SIGIOT);
    signal(SIGTRAP, SimPLiFI_sighandler_SIGTRAP);
    signal(SIGSYS, SimPLiFI_sighandler_SIGSYS);
    // signal(SIGEMT, SimPLiFI_sighandler_SIGEMT);
    return;
}

void SimPLiFI_sighandler_SIGSEGV(int signum) {
    perror("Caught SIGSEGV\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGILL(int signum) {
    perror("Caught SIGILL\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGBUS(int signum) {
    perror("Caught SIGBUS\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGFPE(int signum) {
    perror("Caught SIGFPE\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGABRT(int signum) {
    perror("Caught SIGABRT\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGIOT(int signum) {
    perror("Caught SIGIOT\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGTRAP(int signum) {
    perror("Caught SIGTRAP\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
void SimPLiFI_sighandler_SIGSYS(int signum) {
    perror("Caught SIGSYS\n");
    signal(signum, SIG_DFL);
    raise(signum);
}
// void SimPLiFI_sighandler_SIGEMT(int signum) {
//     perror("Caught SIGEMT\n");
//     signal(signum, SIG_DFL);
//     raise(signum);
// }

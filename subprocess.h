/*
    #04-2017-105-4002
    CMPT300 Project 2
    subprocess.h

    Name: Jetic Gu
    Student ID: 301295754
    I've been working for too long, time for some nuggets.
*/
#ifndef JETIC_SHELL_SUBPROC

void watchBackgroundProcess();
void addBackgroundProcess(int pid);
void clearBackgroundProcess();
void printBackgroundProcess();

void setActiveSubprocess(int pid);
int currentActiveSubprocess();
void pauseActiveSubprocess();
void resumeSubprocess(int pid);
void resumeSubprocessBackground(int pid);

void redirectIO();

// Running State
#define STATE_RUNNING 1
#define STATE_STOPPED -1

#define JETIC_SHELL_SUBPROC
#endif

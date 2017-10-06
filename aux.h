/*
    #04-2017-105-4002
    CMPT300 Project 2
    aux.h

    Name: Jetic Gu
    Student ID: 301295754
    What is life without Chicken Nuggets?
    Wait there is life without Chicken Nuggets?
*/
#ifndef JETIC_SHELL_AUX

void expandHome(char *buff, int maxLen);
void expandEvent(char *buff, int maxLen);

void watchBackgroundProcess();
void addBackgroundProcess(int pid);
void clearBackgoundProcess();
void printBackgoundProcess();

void getHistory(int id, char* buff);
void getLastHistory(char* buff);
void addHistory(char* buff);
void printHistory();
void clearHistory();

#ifdef CHICKEN
int reader(void *buf, size_t nbyte);
int isReading();
#endif

#define JETIC_SHELL_AUX
#endif

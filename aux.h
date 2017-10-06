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

void watchBackgroundProcess();
void addBackgroundProcess(int pid);
void clearBackgoundProcess();

void getHistory();
void addHistory(int id, char* buff);
void printHistory();
void runHistory(int id, char* buff);

#define JETIC_SHELL_AUX
#endif

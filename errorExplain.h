/*
    #04-2017-105-4002
    CMPT300 Project 2
    errorExplain.h

    Name: Jetic Gu
    Student ID: 301295754
    #NuggetLife
*/
#ifndef JETIC_SHELL_ERROR

#include <unistd.h>
#include <errno.h>

char* explainError(int errorCode);
void printErrorMsg(int errorCode);

#define JETIC_SHELL_ERROR
#endif

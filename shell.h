/*
    #04-2017-105-4002
    CMPT300 Project 2
    shell.h

    Name: Jetic Gu
    Student ID: 301295754
    Chicken Nuggets are the best.
*/
#ifndef JETIC_SHELL

#include <limits.h>

// Constants
#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define MAX_STRLEN 1024

// Command execution modes.
#define EXECUTION_CODE int
#define DIRECT_EXECUTION 0001
#define BACKGROUND_EXECUTION 0002
#define BACKGROUND_NO_TRACE_EXECUTION 0003

// Functions
int tokeniseCommand(char *buff, char *tokens[]);
void readCommand(char *buff, char *tokens[]);
int execInternalCommand(char *tokens[]);
void callExecvp(const char *pathname, char *const *argv);
void execSingleCommand(char *tokens[], EXECUTION_CODE executionCode);
void execCommand(char *tokens[]);
void coreExit();

#define JETIC_SHELL
#endif

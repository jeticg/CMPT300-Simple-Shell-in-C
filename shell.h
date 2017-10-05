#ifndef JETIC_SHELL

// Constants
#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)

// Command execution modes.
#define EXECUTION_CODE int
#define DIRECT_EXECUTION 0001
#define BACKGROUND_EXECUTION 0002
#define BACKGROUND_NO_TRACE_EXECUTION 0003

#define JETIC_SHELL
#endif

// Functions
int tokeniseCommand(char *buff, char *tokens[]);
void readCommand(char *buff, char *tokens[], _Bool *inBackground);
int execInternalCommand(char *tokens[]);
void execSingleCommand(char *tokens[], EXECUTION_CODE executionCode);
void execCommand(char *tokens[]);
void coreExit();

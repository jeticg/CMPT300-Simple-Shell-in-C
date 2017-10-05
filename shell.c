// Shell starter file
// You may make any changes to any part of this file.
#include "shell.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Main
int main(int argc, char* argv[]) {
    #ifdef DEBUG
    write(STDOUT_FILENO,
          "Built in DEBUG mode.\n",
          strlen("Built in DEBUG mode.\n"));
    #endif

    char inputBuffer[COMMAND_LENGTH];
    char *tokens[NUM_TOKENS];
    while (true) {
        // Get command
        // Use write because we need to use read() to work with
        // signals, and read() is incompatible with printf().
        write(STDOUT_FILENO, "> ", strlen("> "));
        _Bool inBackground = false;
        readCommand(inputBuffer, tokens, &inBackground);

        // DEBUG: Dump out arguments:
        #ifdef DEBUG
        for (int i = 0; tokens[i] != NULL; i++) {
            write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
            write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
            write(STDOUT_FILENO, "\n", strlen("\n"));
        }
        #endif

        // Execute commands
        execCommand(tokens);
        if (inBackground) {
            #ifdef DEBUG
            write(STDOUT_FILENO, "Run in background.",
                  strlen("Run in background."));
            #endif
        } else {
        }

        /**
         * Steps For Basic Shell:
         * 1. Fork a child process
         * 2. Child process invokes execvp() using results in token array.
         * 3. If inBackground is false, parent waits for
         *    child to finish. Otherwise, parent loops back to
         *    readCommand() again immediately.
         */

    }
    return 0;
}

// Functions
int tokeniseCommand(char *buff, char *tokens[]) {
    /*
        Tokenise the string in 'buff' into 'tokens'.
        buff: Character array containing string to Tokenise.
              Will be modified: all whitespace replaced with '\0'
        tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
                Will be modified so tokens[i] points to the i'th token
                in the string buff. All returned tokens will be non-empty.
                NOTE: pointers in tokens[] will all point into buff!
                      Ends with a null pointer.
        returns: number of tokens.
    */
    int tokenCount = 0;
    _Bool inToken = false;
    int numChars = strnlen(buff, COMMAND_LENGTH);
    for (int i = 0; i < numChars; i++) {
        switch (buff[i]) {
            // Handle token delimiters (ends):
            case '\\':
                // Handle special characters to allow spaces among tokens
                if (i + 1 < numChars) {
                    for (int j = i; j < numChars; j++)
                        buff[j] = buff[j + 1];
                    numChars--;
                    i++;
                    break;
                }
            case ' ':
            case '\t':
            case '\n':
                buff[i] = '\0';
                inToken = false;
                break;

            // Handle other characters (may be start)
            default:
                if (!inToken) {
                    tokens[tokenCount] = &buff[i];
                    tokenCount++;
                    inToken = true;
                }
        }
    }
    tokens[tokenCount] = NULL;
    return tokenCount;
}

void coreExit() {
    exit(0);
}

void readCommand(char *buff, char *tokens[], _Bool *inBackground) {
    /*
        Read a command from the keyboard into the buffer 'buff' and Tokenise it
        such that 'tokens[i]' points into 'buff' to the i'th token in the
        command.
        buff: Buffer allocated by the calling code. Must be at least
              COMMAND_LENGTH bytes long.
        tokens[]: Array of character pointers which point into 'buff'. Must be
                  at least NUM_TOKENS long. Will strip out up to one final '&'
                  token. Tokens will be NULL terminated (a NULL pointer
                  indicates end of tokens).
        inBackground: pointer to a boolean variable. Set to true if user
                      entered an & as their last token; otherwise set to false.
    */
    *inBackground = false;

    // Read input
    int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

    if (length < 0) {
        perror("Unable to read command from keyboard. Terminating.\n");
        exit(-1);
    }

    // Null terminate and strip \n.
    buff[length] = '\0';
    if (buff[strlen(buff) - 1] == '\n') {
        buff[strlen(buff) - 1] = '\0';
    }

    // Tokenise (saving original command string)
    int tokenCount = tokeniseCommand(buff, tokens);
    if (tokenCount == 0) {
        return;
    }

    // Extract if running in background:
    if (tokenCount > 0 && strcmp(tokens[tokenCount - 1], "&") == 0) {
        *inBackground = true;
        tokens[tokenCount - 1] = 0;
    }
}

int execInternalCommand(char *tokens[]) {
    if (tokens[0] == NULL) return 2;
    if (strcmp(tokens[0], "cd") == 0) {
        chdir(tokens[1]);
        return 2;
    }
    if (strcmp(tokens[0], "pwd") == 0) {
        char *buff = getcwd(NULL, 0);
        write(STDOUT_FILENO, buff, strlen(buff));
        write(STDOUT_FILENO, "\n", 1);
        return 2;
    }
    return 0;
}

void execSingleCommand(char *tokens[], EXECUTION_CODE executionCode) {
    /*
        execCommand
        This function will execute a single command.
    */
    #ifdef DEBUG
    write(STDOUT_FILENO, "    Executing Single Command: ",
          strlen("    Executing Single Command: "));
    write(STDOUT_FILENO, tokens[0], strlen(tokens[0]));
    write(STDOUT_FILENO, "\n", 1);
    #endif
    if (strcmp(tokens[0], "exit") == 0 || strcmp(tokens[0], "quit") == 0)
        coreExit();
    if (executionCode == DIRECT_EXECUTION) {
        int pid = fork();
        if (pid < 0) {
            printf("Something is wrong. TAT\n");
            return;
        } else if (pid == 0) {
            if (execInternalCommand(tokens) == 0)
                execvp(tokens[0], tokens);
            exit(0);
        } else {
            wait(NULL);
        }
    } else {
        write(STDOUT_FILENO, "Execution mode not implemented.\n",
              strlen("Execution mode not implemented.\n"));
    }
}

void execCommand(char *tokens[]) {
    /*
        execCommand
        This function will process the tokens into different commmands and execute
        them by calling execSingleCommand.

    */
    // Sort out && symbols-
    char **startOfCommand = &tokens[0];

    for (int i=0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "&&") == 0) {
            char **oldStartOfCommand = startOfCommand;
            startOfCommand = &tokens[i + 1];
            tokens[i] = NULL;
            execSingleCommand(oldStartOfCommand, DIRECT_EXECUTION);
        }
    }
    execSingleCommand(startOfCommand, DIRECT_EXECUTION);
}

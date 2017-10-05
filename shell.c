// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)


/**
 * Command Input and Processing
 */

/*
 * Tokenise the string in 'buff' into 'tokens'.
 * buff: Character array containing string to Tokenise.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokeniseCommand(char *buff, char *tokens[]) {
    int tokenCount = 0;
    _Bool inToken = false;
    int numChars = strnlen(buff, COMMAND_LENGTH);
    for (int i = 0; i < numChars; i++) {
        switch (buff[i]) {
            // Handle token delimiters (ends):
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

/**
 * Read a command from the keyboard into the buffer 'buff' and Tokenise it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * inBackground: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void readCommand(char *buff, char *tokens[], _Bool *inBackground) {
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

void execSingleCommand(char *tokens[]) {

}

void execCommand(char *tokens[]) {

}
/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[]) {
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
        for (int i = 0; tokens[i] != NULL; i++) {

            write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
            write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
            write(STDOUT_FILENO, "\n", strlen("\n"));
            if (strcmp(tokens[i], "exit") == 0) {
                return 0;
            }
        }
        if (inBackground) {
            write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
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

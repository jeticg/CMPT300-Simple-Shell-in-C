#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#ifndef JETIC_SHELL_AUX
#include "aux.h"
#define JETIC_SHELL_AUX
#endif
void expandHome(char *buff, int maxLen) {
    int numChars = (int)strnlen(buff, (size_t)maxLen);
    struct passwd *pw = getpwuid(getuid());
    for (int i = 0; i < numChars; i++) {
        if (buff[i] == '~' && (i == 0 || buff[i-1] == '\0' || buff[i-1] == ' ')) {
            char *homedir = pw->pw_dir;
            for (int j=0; j<(int)strlen(homedir); j++) {
                buff[numChars + (int)strlen(homedir) - j - 1] =\
                    buff[numChars - j];
            }
            numChars--;
            numChars+=strlen(homedir);
            for (int j=0; j<(int)strlen(homedir); j++)
                buff[i + j] = homedir[j];
        }
    }
}

/*
int main() {
    char buff[100];
    strcpy(buff, "cd ~/ABCD");
    expandHome(buff, 100);
    printf("%s\n", buff);
}
*/

/*
    #04-2017-105-4002
    CMPT300 Project 2
    aux.c

    Name: Jetic Gu
    Student ID: 301295754
    I wanna wanna Chicken Nuggets.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>

#include "aux.h"

// DEFINE
#define MAX_STRLEN ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)

// struct
struct Node {
    /*
        Node
        This struct is a linked list for storing child process information.

        value: pid of child process
        id: id of child process in shell
        next: linked list next node
    */
    int value;
    int id;
    struct Node *next;
} *head;

void expandHome(char *buff, int maxLen) {
    /*
    This function expands the '~'s in a command(buff). It allows commands to
    use '~' to indicate home directory.
    */
    int numChars = (int)strnlen(buff, (size_t)maxLen);
    struct passwd *pw = getpwuid(getuid());
    for (int i = 0; i < numChars; i++) {
        if (buff[i] == '~' &&
                (i == 0 || buff[i-1] == '\0' || buff[i-1] == ' ')) {
            char *homedir = pw->pw_dir;
            for (int j=0; j<numChars-i+1; j++) {
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

void addBackgroundProcess(int pid) {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->value = pid;
    newNode->next = NULL;
    if (head == NULL) {
        head = (struct Node*)malloc(sizeof(struct Node));
        head->next = NULL;
        head->id = 0;
        head->value = 0;
    }
    if (head->next == NULL) {
        head->id = 1;
        head->next = newNode;
    } else {
        head->id += 1;
        newNode->next = head->next;
        head->next = newNode;
    }
    newNode->id = head->id;
    char str[MAX_STRLEN];
    snprintf(str, MAX_STRLEN, "[%d] %d\n", newNode->id, pid);
    write(STDOUT_FILENO, str, strlen(str));
}

void watchBackgroundProcess() {
    struct Node *node = head;
    if (head == NULL) return;
    while (node->next != NULL) {
        if (waitpid(node->next->value, NULL, WNOHANG) > 0) {
            printf("[%d] %d Done\n", node->next->id, node->next->value);
            node->next = node->next->next;
        } else {
            node = node->next;
        }
    }
}

void clearBackgoundProcess() {
    while (head != NULL) {
        struct Node *tmp = head;
        head = head->next;
        free(tmp);
    }
}

#ifdef ITTB

int main() {
    char buff[100];
    strcpy(buff, "cd ~/ABCD");
    expandHome(buff, 100);
    printf("%s\n", buff);

    strcpy(buff, "afplay ~/Music/QQXX/CHEMISTRY-Now\ or\ Neve.mp3");
    expandHome(buff, 100);
    printf("%s\n", buff);

    strcpy(buff, "afplay ~/Music/QQ音乐/CHEMISTRY-Now\ or\ Neve.mp3");
    expandHome(buff, 100);
    printf("%s\n", buff);
}
#endif

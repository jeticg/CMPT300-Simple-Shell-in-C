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
#ifdef CHICKEN
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "aux.h"

// DEFINE
#define COMMAND_LENGTH 1024
#define MAX_STRLEN 1024

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
} *head=NULL;

struct CharNode {
    /*
        CharNode
        This struct is a linked list for storing history information.

        value: string
        next: linked list next node
    */
    char *value;
    int id;
    struct CharNode *next;
} *historyHead=NULL;

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

void expandEvent(char *buff, int maxLen) {
    /*
    This function expands the '!'(event)s in a command(buff).
    */
    int numChars = (int)strnlen(buff, (size_t)maxLen);

    for (int i = 0; i < numChars; i++) {

        if (buff[i] == '!' && i + 1 < numChars &&
                (i == 0 || buff[i-1] != '\\')) {

            char *tmp = (char*)malloc((unsigned long)maxLen * sizeof(char));
            int oriSize = 0;
            int shift = 0;

            if (buff[i + 1] == '!') {
                // Previous command
                oriSize = 2;
                getLastHistory(tmp);

            } else {
                // selected command
                int sum = 0;
                oriSize = 1;

                while (i + oriSize < numChars &&
                        '0' <= buff[i + oriSize] &&
                        buff[i + oriSize] <= '9') {
                    sum = sum * 10 + (int)(buff[i + oriSize] - '0');
                    printf("%d %d %d\n", buff[i + oriSize], '0', '9');
                    oriSize++;
                }
                if (oriSize == 1) {
                    write(STDOUT_FILENO, "-shell: event not found\n",
                        strlen("-shell: event not found\n"));
                    strcpy(buff, "");
                    free(tmp);
                    return;
                }
                getHistory(sum, tmp);
                if (strcmp(tmp, "") == 0) {
                    write(STDOUT_FILENO, "-shell: event not found\n",
                        strlen("-shell: event not found\n"));
                    strcpy(buff, "");
                    free(tmp);
                    return;
                }
            }

            // Shift buff (left)
            shift = oriSize;
            for (int j = i; j + shift <= numChars; j++)
                buff[j] = buff[j + shift];
            numChars-=shift;

            // shift buff (right)
            shift = (int)strlen(tmp);
            for (int j=numChars; j>=i; j--) {
                buff[j + shift] = buff[j];
            }
            numChars+=shift;

            // copy
            for (int j=0; j<(int)strlen(tmp); j++)
                buff[i + j] = tmp[j];


            free(tmp);
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
    sprintf(str, "[%d] %d\n", newNode->id, pid);
    write(STDOUT_FILENO, str, strlen(str));
}

void watchBackgroundProcess() {
    struct Node *node = head;
    if (head == NULL) return;
    while (node->next != NULL) {
        int status;
        if (waitpid(node->next->value, &status, WNOHANG) > 0) {
            char str[MAX_STRLEN];
            sprintf(str, "[%d] %d Done(%d)\n",
                node->next->id, node->next->value, status);
            write(STDOUT_FILENO, str, strlen(str));
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


void addHistory(char* buff) {
    #ifdef CHICKEN
    add_history(buff);
    #endif
    struct CharNode *newNode =
        (struct CharNode*)malloc(sizeof(struct CharNode));
    newNode->value = (char*)malloc(COMMAND_LENGTH * sizeof(char));
    strcpy(newNode->value, buff);
    newNode->next = NULL;
    if (historyHead == NULL) {
        historyHead = (struct CharNode*)malloc(sizeof(struct CharNode));
        historyHead->next = NULL;
        historyHead->id = 0;
        historyHead->value = 0;
    }
    if (historyHead->next == NULL) {
        historyHead->id = 1;
        historyHead->next = newNode;
    } else {
        historyHead->id += 1;
        newNode->next = historyHead->next;
        historyHead->next = newNode;
    }
    newNode->id = historyHead->id;
}

void getLastHistory(char* buff) {
    if (historyHead != NULL && historyHead->next != NULL)
        strcpy(buff, historyHead->next->value);
    else
        strcpy(buff, "");
}

void getHistory(int id, char* buff) {
    if (historyHead == NULL || historyHead->next == NULL) {
        strcpy(buff, "");
        return;
    }
    if (id < 1 || id > historyHead->next->id) {
        strcpy(buff, "");
        return;
    }
    struct CharNode *node = historyHead;
    while (node->next) {
        if (node->next->id == id) {
            strcpy(buff, node->next->value);
            return;
        }
        node = node->next;
    }
}

void printHistory() {
    struct CharNode *node = historyHead, *list[10];
    if (node == NULL) return;

    for (int i=0; i<10; i++) list[i] = NULL;
    for (int i=0; i<10 && node->next != NULL; i++) {
        list[i] = node->next;
        node = node->next;
    }
    for (int i=9; i>=0; i--) {
        if (list[i] == NULL) continue;
        char str[MAX_STRLEN];
        sprintf(str, "%d\t", list[i]->id);
        write(STDOUT_FILENO, str, strlen(str));
        write(STDOUT_FILENO, list[i]->value, strlen(list[i]->value));
        write(STDOUT_FILENO, "\n", 1);
    }
}

void clearHistory() {
    while (historyHead != NULL) {
        struct CharNode *tmp = historyHead;
        historyHead = historyHead->next;
        if (tmp->value != NULL)
            free(tmp->value);
        free(tmp);
    }
}

#ifdef CHICKEN
int reader(void *buf, size_t nbyte) {
    char *tmp = getcwd(NULL, 0);
    char *buff = malloc(sizeof(char) * (strlen(tmp) + 3));
    strcpy(buff, tmp);
    buff[strlen(tmp) + 0] = ' ';
    buff[strlen(tmp) + 1] = '>';
    buff[strlen(tmp) + 2] = ' ';
    buff[strlen(tmp) + 3] = '\0';
    char *line = readline(buff);
    strncpy(buf, line, nbyte);
    free(buff);
    return (int)strlen(buf);
}
#endif

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
    return 0;
}
#endif

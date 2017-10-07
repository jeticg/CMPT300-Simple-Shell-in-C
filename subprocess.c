/*
    #04-2017-105-4002
    CMPT300 Project 2
    subprocess.c

    Name: Jetic Gu
    Student ID: 301295754
    I like my Nuggets without sauce.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <limits.h>
#include <signal.h>

#include "subprocess.h"


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


// Internal Functions
struct Node *NewNode() {
    struct Node *newNode =
        (struct Node*)malloc(sizeof(struct Node));
    newNode->id = 0;
    newNode->value = 0;
    newNode->next = NULL;
    return newNode;
}


// Background Processes
void addBackgroundProcess(int pid) {
    /*
    This function adds the pid of a process to the watch list.
    */

    // Parameter check
    if (pid <= 0) return;
    // Check if pid given exists.
    if (getpgid(pid) < 0) return;


    struct Node *newNode = NewNode();
    newNode->value = pid;

    if (head == NULL) head = NewNode();

    if (head->next == NULL) {
        head->id = 1;
    } else {
        head->id += 1;
        newNode->next = head->next;
    }

    head->next = newNode;

    newNode->id = head->id;

    char str[MAX_STRLEN];
    sprintf(str, "[%d] %d\n", newNode->id, pid);
    write(STDOUT_FILENO, str, strlen(str));
}

void watchBackgroundProcess() {
    /*
    This function checks the status of processes in the watch list.
    */
    struct Node *node = head;
    if (head == NULL) return;

    while (node->next != NULL) {
        int status;
        struct Node *tmp;

        if (waitpid(node->next->value, &status, WNOHANG) > 0) {
            // Triggers when process have ended
            char str[MAX_STRLEN];
            if (status != 0)
                sprintf(str, "[%d] %d: Terminated: %d\n",
                    node->next->id, node->next->value, status);
            else
                sprintf(str, "[%d] %d: Done\n",
                    node->next->id, node->next->value);
            write(STDOUT_FILENO, str, strlen(str));

            // Remove node
            tmp = node->next;
            node->next = tmp->next;
            free(tmp);

        } else {
            // All is well, move on to the next
            node = node->next;
        }
    }
}

void clearBackgroundProcess() {
    /*
    This function kills all processes in the watch list and clear the list.
    */
    watchBackgroundProcess();
    while (head != NULL) {
        struct Node *tmp = head;
        head = head->next;
        if (tmp->value != 0) {
            // Print kill message
            char str[MAX_STRLEN];
            sprintf(str, "[%d] ", tmp->id);
            write(STDOUT_FILENO, str, strlen(str));
            sprintf(str, "%d: Killed\n", tmp->value);
            write(STDOUT_FILENO, str, strlen(str));

            // perform the kill
            kill(tmp->value, 9);
            free(tmp);
        }
    }
}

void printBackgroundProcess() {
    /*
    This function prints the watch list. Only the last 10 commands will be
    printed.
    */
    watchBackgroundProcess();
    struct Node *node = head, *list[10];
    if (node == NULL) return;

    // Initialise print list to all NULL
    for (int i=0; i<10; i++) list[i] = NULL;

    // Fill the list
    for (int i=0; i<10 && node->next != NULL; i++) {
        list[i] = node->next;
        node = node->next;
    }

    // Print
    for (int i=9; i>=0; i--) {
        // Ignore empty entries
        if (list[i] == NULL) continue;

        // Print entry
        char str[MAX_STRLEN];
        sprintf(str, "[%d] ", list[i]->id);
        write(STDOUT_FILENO, str, strlen(str));
        sprintf(str, "%d", list[i]->value);
        write(STDOUT_FILENO, str, strlen(str));
        write(STDOUT_FILENO, "\n", 1);
    }
}

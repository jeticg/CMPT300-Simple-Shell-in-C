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
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>

#include "errorExplain.h"
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
    int state;
    int id;
    struct Node *next;
} *head=NULL;


// Internal Functions
struct Node *NewNode() {
    struct Node *newNode =
        (struct Node*)malloc(sizeof(struct Node));
    newNode->id = 0;
    newNode->value = 0;
    newNode->state = 0;
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

    // Check if already in list
    struct Node *newNode = head;
    if (newNode != NULL) {
        newNode = newNode->next;
        while (newNode != NULL) {
            if (newNode->value == pid) {
                char str[MAX_STRLEN];
                sprintf(str, "[%d] %d\n", newNode->id, pid);
                write(STDOUT_FILENO, str, strlen(str));
                return;
            }
            newNode = newNode->next;
        }
    }

    newNode = NewNode();
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
    newNode->state = STATE_RUNNING;

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

    if (head->value != 0 && getpgid(head->value) < 0)
        head->value = 0;

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
        }
        free(tmp);
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

        if (list[i]->state == STATE_RUNNING)
            write(STDOUT_FILENO, "running ", strlen("running "));
        if (list[i]->state == STATE_STOPPED)
            write(STDOUT_FILENO, "stopped ", strlen("stopped "));

        sprintf(str, "%d", list[i]->value);
        write(STDOUT_FILENO, str, strlen(str));
        write(STDOUT_FILENO, "\n", 1);
    }
}

void setActiveSubprocess(int pid) {
    /*
    This function sets currently active (foreground) process to pid
    */
    // Parameter check
    if (pid <= 0) return;
    // Check if pid given exists.
    if (getpgid(pid) < 0) return;

    if (head == NULL) head = NewNode();
    head->value = pid;
}

int currentActiveSubprocess() {
    /*
    Returns the PID of currently active foreground process.
    */
    if (head == NULL) return 0;
    if (head->value <= 0) return 0;
    int pid = head->value;
    if (getpgid(pid) < 0) {
        head->value = 0;
        return 0;
    }
    return pid;
}

void pauseActiveSubprocess() {
    /*
    This function marks the currently active process as stopped and sends it a
    SIGSTOP signal.
    */
    if (head == NULL) return;
    if (head->value <= 0) return;
    int pid = head->value;
    if (getpgid(pid) < 0) {
        head->value = 0;
        return;
    }
    if (kill(pid, SIGSTOP) < 0) {
        write(STDOUT_FILENO, "kill: ", strlen("kill: "));
        printErrorMsg(errno);
        return;
    }
    addBackgroundProcess(pid);
    head->next->state = STATE_STOPPED;
    head->value = 0;
}

void resumeSubprocess(int pid) {
    /*
    This function resumes a previously stopped process to foreground.
    If pid equals 0 then the first process in the process list that is stopped
    will be resumed.
    */
    struct Node *node;

    watchBackgroundProcess();
    if (head == NULL) return;
    if (pid == 0) {
        node = head->next;
        while (node != NULL) {
            if (node->state == STATE_STOPPED) {
                pid = node->value;
                break;
            }
        }
    }
    if (pid <= 0) return;


    if (kill(pid, SIGCONT) < 0) {
        write(STDOUT_FILENO, "-rsum: ", strlen("-rsum: "));
        printErrorMsg(errno);
        return;
    }
    // Set state in list
    node = head->next;
    while (node != NULL) {
        if (node->value == pid) {
            node->state = STATE_RUNNING;
            char str[MAX_STRLEN];
            sprintf(str, "[%d] %d\n", node->id, pid);
            write(STDOUT_FILENO, str, strlen(str));
            break;
        }
    }
    setActiveSubprocess(pid);
    while (pid == currentActiveSubprocess() && getpgid(pid) >= 0)
        waitpid(pid, NULL, WNOHANG);
}


void redirectIO() {
    /*
    This function redirects all IO to /dev/null, which is nowhere.
    This is for background processes obviously, to prevent them from messing up
    the console.
    */
    int fin = open("/dev/null", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    int fout = open("/dev/null", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    dup2(fin, STDIN_FILENO); // make stdin  go to file
    dup2(fout, STDOUT_FILENO); // make stdout go to file
    dup2(fout, STDERR_FILENO); // make stderr go to file

    close(fin);
    close(fout);
}

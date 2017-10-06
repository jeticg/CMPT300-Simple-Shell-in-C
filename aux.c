/*
    #04-2017-105-4002
    CMPT300 Project 2
    aux.c

    Name: Jetic Gu
    Student ID: 301295754
    I wanna wanna Chicken Nuggets.
*/
/*
    So what on earth is CHICKEN option?
    CHICKEN allows one to utilise readline to read from keyboard. There are
    many reasons to why one would want to do this, but in short, it allows easy
    auto-completion(not perfect, sometimes doesn't work) and arrow keys for
    editing. Although one could implement ones own, it is time consuming and
    not worthwhile.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>
#include <signal.h>

#ifdef CHICKEN
#include <readline/readline.h>
#include <readline/history.h>
int READING = 0;
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


// Internal Functions
struct Node *NewNode() {
    struct Node *newNode =
        (struct Node*)malloc(sizeof(struct Node));
    newNode->id = 0;
    newNode->value = 0;
    newNode->next = NULL;
    return newNode;
}
struct CharNode *NewCharNode() {
    struct CharNode *newNode =
        (struct CharNode*)malloc(sizeof(struct CharNode));
    newNode->id = 0;
    newNode->value = NULL;
    newNode->next = NULL;
    return newNode;
}


// Functions
void expandHome(char *buff, int maxLen) {
    /*
    This function expands the '~'s in a command(buff). It allows commands to
    use '~' to indicate home directory.
    */
    int numChars = (int)strnlen(buff, (size_t)maxLen);

    for (int i = 0; i < numChars; i++) {
        if (buff[i] == '~' &&
                (i == 0 || buff[i-1] == '\0' || buff[i-1] == ' ')) {

            // Get home directory
            struct passwd *pw = getpwuid(getuid());
            char *homedir;
            if (pw != NULL && pw->pw_dir != NULL)
                homedir = pw->pw_dir;
            else
                homedir = "";
            int homedirLen = (int)strnlen(homedir, (size_t)maxLen);

            // Safety check: result not exceed maximum command length
            if (numChars - 1 + homedirLen >= maxLen) {
                buff[0] = '\0';
                write(STDOUT_FILENO,
                    "-shell: command exceeded maximum length\n",
                    strlen("-shell: command exceeded maximum length\n"));
                return;
            }

            for (int j=0; j<numChars-i+1; j++) {
                buff[numChars + homedirLen - 1 - j] =  buff[numChars - j];
            }
            numChars--;
            numChars+=strlen(homedir);
            for (int j=0; j<homedirLen; j++)
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

void getPrompt(char **buff) {
    // Check parameter
    if (buff == NULL) return;

    // Get home directory
    char *tmp = getcwd(NULL, 0);
    if (tmp == NULL) tmp = "";

    // Add trailing " > "
    *buff = malloc(sizeof(char) * (strlen(tmp) + 3));
    char *promt = *buff;
    strcpy(promt, tmp);
    promt[strlen(tmp) + 0] = ' ';
    promt[strlen(tmp) + 1] = '>';
    promt[strlen(tmp) + 2] = ' ';
    promt[strlen(tmp) + 3] = '\0';
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

void clearBackgoundProcess() {
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

void printBackgoundProcess() {
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


// History
void addHistory(char* buff) {
    /*
    This function adds buff to the history list. Buff is a string.
    The important thing here is that this function is not limited by the length
    of buff, as memories are dynamically allocated.
    */
    // Parameter check
    if (buff == NULL) return;
    if (buff[0] == '\0') return;
    // Safety check (#1)
    if (strlen(buff) > COMMAND_LENGTH) return;

    #ifdef CHICKEN
    add_history(buff);
    #endif

    struct CharNode *newNode = NewCharNode();
    newNode->value = (char*)malloc((strlen(buff) + 1) * sizeof(char));
    strcpy(newNode->value, buff);

    if (historyHead == NULL) historyHead = NewCharNode();

    if (historyHead->next == NULL) {
        historyHead->id = 1;
    } else {
        historyHead->id += 1;
        newNode->next = historyHead->next;
    }

    historyHead->next = newNode;
    newNode->id = historyHead->id;
}

void getLastHistory(char* buff) {
    /*
    This function prints the last command to buff. It calls getHistory to make
    it work.
    */
    // (#1)
    if (historyHead == NULL || historyHead->next == NULL)
        // Parameter check
        getHistory(-1, buff);
    else
        getHistory(historyHead->next->id, buff);
}

void getHistory(int id, char* buff) {
    /*
    This function returns the command matching id. Might not be very safe to
    use strcpy though.
    */
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
    /*
    This function prints last 10 lines in history.
    */
    struct CharNode *node = historyHead, *list[10];
    if (node == NULL) return;

    // Fill list with NULL
    for (int i=0; i<10; i++) list[i] = NULL;

    // Fill nodes with history
    for (int i=0; i<10 && node->next != NULL; i++) {
        list[i] = node->next;
        node = node->next;
    }

    // Print
    for (int i=9; i>=0; i--) {
        // Ignore NULL entries
        if (list[i] == NULL) continue;
        // Actual print
        char str[MAX_STRLEN];
        sprintf(str, "%d\t", list[i]->id);
        write(STDOUT_FILENO, str, strlen(str));
        write(STDOUT_FILENO, list[i]->value, strlen(list[i]->value));
        write(STDOUT_FILENO, "\n", 1);
    }
}

void clearHistory() {
    /*
    This function clears all history records.
    */
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
    READING = 1;

    // Preparing the prompt
    char *prompt;
    getPrompt(&prompt);

    // Readline
    char *line = readline(prompt);
    // Upon success line should be str. But in case of EOF, it will be NULL
    if (line == NULL) {
        // When EOF occurs, no blank lines are printed. So we need to do so
        // manually.
        strncpy(buf, "", nbyte);
        write(STDOUT_FILENO, "\n", 1);
    }
    else strncpy(buf, line, nbyte);

    free(prompt);
    READING = 0;
    return (int)strlen(buf);
}
int isReading() {
    return READING;
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

#ifndef JETIC_SHELL_AUX
void expandHome(char *buff, int maxLen);

void watchBackgroundProcess();
void addBackgroundProcess(int pid);
void clearBackgoundProcess();

void getHistory();
void addHistory(int id, char* buff);
void printHistory();
void runHistory(int id, char* buff);
#define JETIC_SHELL_AUX
#endif

#ifndef JETIC_SHELL_ERROR

#include <unistd.h>
#include <errno.h>

#define JETIC_SHELL_ERROR
#endif

char* explainError(int errorCode);
void printErrorMsg(int errorCode);

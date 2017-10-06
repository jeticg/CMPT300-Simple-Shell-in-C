#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "errorExplain.h"

#define MAX_STRLEN ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)


char* explainError(int errorCode) {
    if (errorCode == E2BIG)
        return "The total number of bytes in the environment (envp) and argument list (argv) is too large";

    if (errorCode == EACCES)
        return "Access denied";

    if (errorCode == EFAULT)
        return "filename points outside your accessible address space";

    if (errorCode == EINVAL)
        return "An ELF executable had more than one PT_INTERP segment (i.e., tried to name more than one interpreter)";

    if (errorCode == EIO)
        return "An I/O error occurred";

    if (errorCode == EISDIR)
        return "An ELF interpreter was a directory";

    // This error code appears to be undeclared for some reason
    // if (errorCode == ELIBBAD)
    //    return "An ELF interpreter was not in a recognized format";

    if (errorCode == ELOOP)
        return "Too many symbolic links were encountered in resolving filename or the name of a script or ELF interpreter";

    if (errorCode == EMFILE)
        return "The process has the maximum number of files open";

    if (errorCode == ENAMETOOLONG)
        return "filename is too long";

    if (errorCode == ENFILE)
        return "The system limit on the total number of open files has been reached";

    if (errorCode == ENOENT)
        return "The file filename or a script or ELF interpreter does not exist, or a shared library needed for file or interpreter cannot be found";

    if (errorCode == ENOEXEC)
        return "An executable is not in a recognized format, is for the wrong architecture, or has some other format error that means it cannot be executed";

    if (errorCode == ENOMEM)
        return "Insufficient kernel memory was available";

    if (errorCode == ENOTDIR)
        return "A component of the path prefix of filename or a script or ELF interpreter is not a directory";

    if (errorCode == EPERM)
        return "The file system is mounted nosuid or the process is being traced, the user is not the superuser, and the file has the set-user-ID or set-group-ID bit set";

    if (errorCode == ETXTBSY)
        return "Executable was open for writing by one or more processes";

    if (errorCode == EBADF)
        return "fd is not a valid file descriptor.";
    return "Unknown error code";
}

void printErrorMsg(int errorCode) {
    write(STDERR_FILENO, "Error code ", strlen("Error code "));
    char str[MAX_STRLEN];
    snprintf(str, MAX_STRLEN, "%d", errorCode);
    write(STDOUT_FILENO, str, strlen(str));
    write(STDOUT_FILENO, ", ", 2);
    char *msg = explainError(errorCode);
    write(STDOUT_FILENO, msg, strlen(msg));
    write(STDOUT_FILENO, "\n", 1);
}

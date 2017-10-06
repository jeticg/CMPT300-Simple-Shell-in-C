/*
    #04-2017-105-4002
    CMPT300 Project 2
    errorExplain.c

    Name: Jetic Gu
    Student ID: 301295754
    KFC is good, but not as good and Chicken Nuggets.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "errorExplain.h"

#define MAX_STRLEN 1024


char* explainError(int errorCode) {
    switch(errorCode) {
        case EADDRINUSE:
            return "Address already in use.";

        case EADDRNOTAVAIL:
            return "Address not available.";

        case EAFNOSUPPORT:
            return "Address family not supported.";

        case EAGAIN:
            return "Resource temporarily unavailable.";

        case EALREADY:
            return "Connection already in progress.";

        case EBADF:
            return "Bad file descriptor.";

        case EBADMSG:
            return "Bad message.";

        case EBUSY:
            return "Device or resource busy.";

        case ECANCELED:
            return "Operation canceled.";

        case ECHILD:
            return "No child processes.";

        case ECONNABORTED:
            return "Connection aborted.";

        case ECONNREFUSED:
            return "Connection refused.";

        case ECONNRESET:
            return "Connection reset.";

        case EDEADLK:
            return "Resource deadlock avoided.";

        case EDESTADDRREQ:
            return "Destination address required.";

        case EDOM:
            return "Mathematics argument out of domain of function.";

        case EDQUOT:
            return "Disk quota exceeded.";

        case EEXIST:
            return "File exists.";

        case EFAULT:
            return "Bad address.";

        case EFBIG:
            return "File too large.";

        case EHOSTUNREACH:
            return "Host is unreachable.";

        case EIDRM:
            return "Identifier removed.";

        case EILSEQ:
            return "Invalid or incomplete multibyte or wide character.";

        case EINPROGRESS:
            return "Operation in progress.";

        case EINTR:
            return "Interrupted function call.";

        case EINVAL:
            return "Invalid argument.";

        case EIO:
            return "Input/output error.";

        case EISCONN:
            return "Socket is connected.";

        case EISDIR:
            return "Is a directory.";

        case ELOOP:
            return "Too many levels of symbolic links.";

        case EMFILE:
            return "Too many open files.";

        case EMLINK:
            return "Too many links.";

        case EMSGSIZE:
            return "Message too long.";

        case EMULTIHOP:
            return "Multihop attempted.";

        case ENAMETOOLONG:
            return "Filename too long.";

        case ENETDOWN:
            return "Network is down.";

        case ENETRESET:
            return "Connection aborted by network.";

        case ENETUNREACH:
            return "Network unreachable.";

        case ENFILE:
            return "Too many open files in system.";

        case ENOBUFS:
            return "No buffer space available.";

        case ENODATA:
            return "No message is available on the STREAM head read queue.";

        case ENODEV:
            return "No such device.";

        case ENOENT:
            return "No such file or directory.";

        case ENOEXEC:
            return "Exec format error.";

        case ENOLCK:
            return "No locks available.";

        case ENOLINK:
            return "Link has been severed.";

        case ENOMEM:
            return "Not enough space.";

        case ENOMSG:
            return "No message of the desired type.";

        case ENOPROTOOPT:
            return "Protocol not available.";

        case ENOSPC:
            return "No space left on device.";

        case ENOSR:
            return "No STREAM resources.";

        case ENOSTR:
            return "Not a STREAM.";

        case ENOSYS:
            return "Function not implemented.";

        case ENOTCONN:
            return "The socket is not connected.";

        case ENOTDIR:
            return "Not a directory.";

        case ENOTEMPTY:
            return "Directory not empty.";

        case ENOTSOCK:
            return "Not a socket.";

        case ENOTTY:
            return "Inappropriate I/O control operation.";

        case ENXIO:
            return "No such device or address.";

        case EOPNOTSUPP:
            return "Operation not supported on socket.";

        case EOVERFLOW:
            return "Value too large to be stored in data type.";

        case EPERM:
            return "Operation not permitted.";

        case EPIPE:
            return "Broken pipe.";

        case EPROTO:
            return "Protocol error.";

        case EPROTONOSUPPORT:
            return "Protocol not supported.";

        case EPROTOTYPE:
            return "Protocol wrong type for socket.";

        case ERANGE:
            return "Result too large.";

        case EROFS:
            return "Read-only filesystem.";

        case ESPIPE:
            return "Invalid seek.";

        case ESRCH:
            return "No such process.";

        case ESTALE:
            return "Stale file handle.";

        case ETIME:
            return "Timer expired.";

        case ETIMEDOUT:
            return "Connection timed out.";

        case ETXTBSY:
            return "Text file busy.";

        case EXDEV:
            return "Improper link.";

        default:
            return "Unknown error code";
    }
}

void printErrorMsg(int errorCode) {
    write(STDERR_FILENO, "Error code ", strlen("Error code "));
    char str[MAX_STRLEN];
    sprintf(str, "%d", errorCode);
    write(STDOUT_FILENO, str, strlen(str));
    write(STDOUT_FILENO, ", ", 2);
    char *msg = explainError(errorCode);
    write(STDOUT_FILENO, msg, strlen(msg));
    write(STDOUT_FILENO, "\n", 1);
}

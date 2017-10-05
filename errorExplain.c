#include "errorExplain.h"


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
    return "Unknown error code";
}
/*
E2BIG
The total number of bytes in the environment (envp) and argument list (argv) is too large.

EACCES

Search permission is denied on a component of the path prefix of filename or the name of a script interpreter. (See also path_resolution(7).)

EACCES

The file or a script interpreter is not a regular file.

EACCES

Execute permission is denied for the file or a script or ELF interpreter.

EACCES

The file system is mounted noexec.

EFAULT

filename points outside your accessible address space.

EINVAL

An ELF executable had more than one PT_INTERP segment (i.e., tried to name more than one interpreter).

EIO

An I/O error occurred.

EISDIR

An ELF interpreter was a directory.

ELIBBAD
An ELF interpreter was not in a recognized format.
ELOOP
Too many symbolic links were encountered in resolving filename or the name of a script or ELF interpreter.

EMFILE

The process has the maximum number of files open.

ENAMETOOLONG
filename is too long.
ENFILE
The system limit on the total number of open files has been reached.

ENOENT

The file filename or a script or ELF interpreter does not exist, or a shared library needed for file or interpreter cannot be found.

ENOEXEC
An executable is not in a recognized format, is for the wrong architecture, or has some other format error that means it cannot be executed.
ENOMEM
Insufficient kernel memory was available.

ENOTDIR
A component of the path prefix of filename or a script or ELF interpreter is not a directory.
EPERM
The file system is mounted nosuid, the user is not the superuser, and the file has the set-user-ID or set-group-ID bit set.

EPERM

The process is being traced, the user is not the superuser and the file has the set-user-ID or set-group-ID bit set.

ETXTBSY
Executable was open for writing by one or more processes.
*/

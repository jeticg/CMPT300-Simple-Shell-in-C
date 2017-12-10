# CMPT300-Projekt 2:
![Fun](https://travis-ci.org/jeticg/Simple-Shell-in-C.svg?branch=master)
# A very simple shell written in C

## 0. Dev environment

This Shell is developed chiefly under Mac OS X(why change it to macOS?)
10.13, and compiled and tested under Ubuntu 16.04 for compatibility. Most of
the stuff should be standard libraries specified in POSIX with exception to
`readline` package, which could require extra library.

To disable `readline`, change compile option in `Makefile` and remove option

    -DCHICKEN

and remove linker option

    -lreadline

The difference is that readline would allow you to use arrow keys when entering
commands, nothing else in this implementation uses readline.

## 1. Compile and running

    > make clean && make
    > ./shell

## 2. Implemented features

### 2.1 Tokenisation of command

Commands are read as single lines. Maximum length is 1024 by default but one
could change it of course. When a command is read, the tokeniser breaks it into
separate tokens (arguments).

The tokeniser deals with the following scenarios:

1. Whitespace are identified to separate tokens.

2. '\' will result in treating the next character as standard character
(whitespaces will be treated as characters as well instead of separators).

3. '!' are treated as event prefixes. '!!' will be replaced by the
previous command(if doesn't exist will simply be removed). '!' followed by
a number will be replaced by the corresponding command in history. History
goes back to as ancient as possible (not limited to 10). See `aux.c` for
implementation detail.

4. '~' after a whitespace will be expanded to user home dir.

5. Quotation marks are dealt with. Characters in inclusive quotation marks
will be treated as a elements of a single token. Single quotation marks and 
double quotation marks are handled separately. For example, the tokens of the 
command:

	"''"'""'"'"'"' (Singles in doubles; doubles in singles; single in 
					doubles; double in singles)

	is:

	''""'" (Singles; doubles; single; double)
	
6. '\n' always ends a command.

### 2.2 Execution

A single line command can launch a lot of separate processes. Each is ended
by a '\n' or '&' or '&&'. 

1. '&&' means the following command will not be executed until this command
is finished. (Unlike standard shell, this shell don't care if it ended
successfully or was it killed.)

2. '&' means the current command will be executed in background, not blocking
the next command.

#### 2.2.1 Background execution

Background processes will not be interrupted by `SIGINT` nor `SIGTSTP` signals.
Their outputs will not be displayed and they will not be interactive(cannot)
receive keyboard input. This IO redirection is done in 
`subprocess.c/redirectIO()` using `dup2`.

Background processes will be added into a list, which will be monitored if
they exits. List of background process can be printed using "subproc" command.

### 2.2.2 Stopping and resuming a foreground process

Foreground process can be stopped by pressing Ctrl-Z just like in any shell.
After being stopped the process will be added into the background list. You
can stop as many processes as you would like. The last stopped process in the
list (the last to be stopped) can be resumed using "rsum" command. Sending
`SIGCONT` using other methods is not monitored by this shell.

#### 2.2.3 Input/Output redirection is not implemented

I have already spent a day (about 24 hours in 2 days time) writing this shell
and I am tired. (Yeah, I am lazy, so what? A non-lazy programmer is not a good
programmer.)

### 2.3 History

History is printed using "history" command. Unlike normal shells, this command
only prints the last 10 commands (somehow this is the instructor's requirement,
if I could have it my way I would do it like in a standard shell). But all
histories are remembered and can be accessed using "!" event. I might change it
to behave like normal shells in future, but I do not promise.

### 2.4 Prompt

One can customise ones prompt by modifying `aux.c/expandHome()`. What I did was
per instructor's requirement and prints out the current working directory.

### 2.5 Error output

I wrote my own `errno` explainer because `explain_errno` doesn't work on OS X.
In the event that a foreground process doesn't end well, error code will be
displayed along with explanation. I only wrote explanations for error code
shared by OS X and Linux, excluding ones with duplicated values and ones not 
recognised by the compiler(don't know why).

### 2.6 Internal commands

Internal commands differ from other commands, in that they are not executed as
subprocesses and that they are written by me.

Internal commands include:

1. `cd`. Change directory just like any other shell. `cd` without arguments will
take you home(assuming your home can be found on earth, not that I am racist to
Martians, you guys are cool).

2. `pwd`. Prints the current work directory (which prompt displays anyway).

3. `history`. Prints out last 10 commands. See section 2.3 for detail.

4. `subproc`. Prints out all subprocesses that are running or stopped.

5. `rsum`. Resume last stopped subprocess. See section 2.2.2 for detail.

## 3. Signal handling

The following signals are handled:

1. `SIGINT`. This will send `SIGINT` to currently running process (kill it) and
display history as the instructor requires.

2. `SIGTSTP`. Stops currently active process and put it into background. Details
on stopping and resuming is covered in section 2.2.2.

## 4. Disclaimer

I wrote the whole thing myself, with exception to the function provided in the
sample (which I just about totally rewrote anyway).

If you submit my code as YOUR homework there's nothing I can do as GitHub is a
free social network service. But you do it at your own risk and I strongly
despise such action. I will take you to court in the after life unless you bribe
me with chicken nuggets (preferably from McDonald's, no sauce).


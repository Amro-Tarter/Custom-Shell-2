# Custom-Shell-2
Custom Shell Project

Overview

This project implements a custom shell that allows users to execute commands with support for background processes, command chaining using || and &&, and redirection of standard error output. It also maintains a list of running background jobs.

Features

Execute system commands

Support for background execution using &

Command chaining with || and &&

Redirection of standard error output using 2>

Job management with jops command to list background jobs

Handles memory allocation errors gracefully

Signal handling for terminated background processes

Installation

Clone the repository:

git clone <repository-url>
cd <repository-folder>

Compile the program:

gcc -o custom_shell custom_shell.c

Run the shell:

./custom_shell

Usage

Run commands normally:

ls -l

Run commands in the background:

sleep 10 &

Check background jobs:

jops

Use command chaining:

ls && echo "Success"
ls || echo "Failed"

Redirect standard error output:

ls non_existent_folder 2> error_log.txt

Exit the shell:

exit_shell

Implementation Details

Commands are tokenized and executed using fork() and execvp().

Background jobs are tracked using a linked list.

Signal handling is implemented to clean up terminated jobs.

Limitations

Supports a maximum of 11 words per command.

Only one level of command chaining (|| or &&) is supported at a time.

Future Improvements

Add support for piping (|).

Implement built-in commands like cd.

Improve alias and script handling.

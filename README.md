# shell-program

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Functionality](#functionality)
- [Installation](#installation)
- [Testing](#testing)
- [Known Bugs](#known-bugs)

## Description

This project implements a POSIX-like shell program that utilizes C code and the POSIX command language to implement a large subset of the POSIX shell’s functionality. The program houses a large amount of high-level functionalities, including shell-based commands/utilities, signal handling behaviors, command execution, I/O redirection, variable assignment, pipelining between commands in the shell, and job control. The program was created on top of skeleton code provided by Oregon State University.

## Features

- **Command-Line Parsing**: Parses command-line input into commands to be executed
- **External Command Execution**: Executes a variety of external commands (programs) as separate processes.
- **Built-in Commands**: Implements a variety of shell built-in commands within the shell itself.
- **I/O Redirection**: Performs a variety of I/O redirections on behalf of commands to be executed.
- **Environment Variables**: Allows assigning, evaluating, and exporting variables to the shell environment.
- **Signal Handling**: Implements signal handling appropriate for the shell and most executed commands.
- **Job Control**: Supports managing processes and pipelines of processes using job control.

## Functionality

The entry point of the shell program can be found in the module ``bigshell``, which contains the function housing the Main Event Loop, which utilizes the Read Evaluate Print Loop method. For a more in-depth explanation on what each module of the code does, refer to the in-line comments at the top of each ``.c`` file.

## Installation

To build and run the shell program, follow these steps:

1. Clone the repository.
2. Compile the code using the ``make`` command from the root directory.
3. Navigate to the ``release`` directory and run the program with ``./bigshell``.

## Testing

A reference implementation of the completed program is provided in ``reference/bigshell`` for troubleshooting purposes. Compiling the code with ``make debug`` creates an alternative version of the code in the ``debug`` directory which includes assertions and debugging messages. All comments from the skeleton code have been kept included in the code to assist with future debugging.

## Known Bugs

There is a bug involving forking in runner.c, affected by line 563, where the process forks based on what type of command it is. The process is supposed to fork if the command is not a buitin command, or not a foreground command. The bug affects the exit status of the shell when the exit builtin command is run. When the conditional statement on line 563 is ``!is_builtin || is_bg``, the exit status works properly, and the exit status is set appropriately to the status passed with the exit command. However, this causes most builtins outside of exit to stop working. Alternatively, when the condition is set to ``!is_builtin && !is_fg``, the exit status is always 0 after the exit builtin is run, but all builtins work properly. This bug is being actively worked on, but has yet to be solved.

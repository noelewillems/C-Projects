/* 
Created by Noel Willems: a simple shell program
Sources:
    https://brennan.io/2015/01/16/write-a-shell-in-c/

Notes:
- Loops until user exits.

Main:
while (!exit)
    > Prompt for cmd (512 chars max)
    > Parse input
    > Execute cmd
        if (cmd = exit)
            exit


1. cd changeDirectory() 
    - goes up a level in the directory
    - if there is no further level, print an error message

2. executables
if there is a command that is not a method, then assume it's an executable.
use fork/exec to run the program.
if program does not exist, don't run it.
>>>Structures Needed<<<
    - fork / exec

3. pwd history()
Limit history to 10 cmds. Only output 10 cmds.
    - shows history of cmds, numbered 0 --> (starting with pwd)
    - if user enters !! after pwd, run the previous cmd in history (if no prev cmd, error)
    - if user enters ! <n>, run the nth cmd in history (if invalid n, error)
>>>Structures Needed<<<
    - Array of cmds as strings
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#define INPUT_BUFF_SIZE 512

void changeDir(char* line) {
    printf("Change directories.\n");
    chdir("..");
}

void exe(char* line) {
    printf("Execute a cmd.\n");
}

// cmd1 is the "actual" cmd
void executeCmd(char* line, char* cmd) {
    printf("line: %s\n", line);
    printf("cmd: %s\n", cmd);
    if(strcmp(cmd, "cd") == 0) {
        changeDir(line);
    }
    else {
        exe(line);
    }
}

// Returns a 1 if the command is "exit"
int parseCmd(char* input) {
    // Must create temp array to hold original input before strtok changes it
    char line[INPUT_BUFF_SIZE * sizeof(char)];
    strcpy(line, input);
    // Getting cmd
    char *cmd = strtok(input, " ");

    if(strcmp(cmd, "exit") == 0) {
        return 1;
    } else {
        // Send original input line to executeCmd
        executeCmd(line, cmd);
    }
    return 0;
}

// Method that runs until user specifies exit.
void run() {
    // exit = 1 means exit; exit = 0 means don't exit
    int exit = 0;
    char *input = malloc(INPUT_BUFF_SIZE * sizeof(char));
    while(exit == 0) {
        printf("> ");
        fgets(input, INPUT_BUFF_SIZE, stdin) ;
        // Null-terminating input
        input[strlen(input) - 1] = '\0';
        // Exit flag
        exit = parseCmd(input);
        printf("===================\n");
    }
}

int main(int argc, char* argv[]) {
    run();
    return(EXIT_SUCCESS);
}
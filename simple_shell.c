/* 
Created by Noel Willems: a simple shell program
Sources:
    Concept of various methods (no code taken from this site): https://brennan.io/2015/01/16/write-a-shell-in-c/
    Other sources listed in respective sections.


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
    - Array of cmds and their aliases - so aliases and which method they go to? Maybe?

    ALL COMPONENTS:
    [x] main - main()
    [x] run - run()
    [x] parseCmd - parseCmd()
    [x] doCmd - do Cmd()
    executeProg - <program_name> <args>
    [x] changeDir - cd
    [x] listFiles - ls
    showHistory - pwd
    alias

    TO DO
    1. ls
    2. pwd 
    3. alias
    4. execute
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#define INPUT_BUFF_SIZE 512
#define numCmds 100
// Global variable for history - initialized to 100, BUT pwd will truncate to 10.
// Array of pointers to char arrays. 
char* history[numCmds];
// Index tracker
int historyIndex = 0;

void alias() {

}

void showHistory() {

}

// ls: list files in current directory.
// Source: https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
void listFiles() {
    struct dirent *de;
    DIR *directory = opendir("."); 
    if(directory == NULL) {
        printf("Could not open the current directory.\n");
    }
    while ((de = readdir(directory)) != NULL) {
        printf("%s\n", de->d_name); 
    } 
    closedir(directory);     
}

// cd: change directories specified by args
void changeDir(char* args) {
    printf("args: %s\n", args);
    char s[100];
    // Path name
    char* pathName = malloc(512 * sizeof(char));
    // Get the current working directory path name
    getcwd(s, 100);
    // Assigning the cwd to the pathname
    int a = 0;
    for(int i = 0; i < strlen(s); i++) {
        *(pathName + a) = s[i];
        a++;
    }
    // Appending the args to the pathname
    int b = strlen(s);
    for(int i = 0; i < strlen(args); i++) {
        *(pathName + b) = args[i];
        b++;
    }  
    // If no path is provided
    if(args == NULL) {
        printf("Error: expected argument to change directories.\n");
    }
    // If they want to go up a level and do ".."
    if(strcmp(args, "..") == 0) {
        printf("Going up a level.\n");
        chdir("..");
    // If chdir succeeds
    } else if(chdir(pathName) == 0) {
        printf("Success!\n");
    // If chdir fails
    } else if(chdir(args) != 0) {
        perror("");
    }
}

// Execute programs with fork/exec - cmd is the program name; args are... the args
void exe(char* cmd, char* args) {
    printf("Execute a cmd.\n");
}

// Send cmds to their respective methods
void doCmd(char* cmd, char* args) {
    // Change dir
    if(strcmp(cmd, "cd") == 0) {
        changeDir(args);
    }
    // List files
    if(strcmp(cmd, "ls") == 0) {
        listFiles();
    }
    // If it's none of the above, assume it's an executable.
    else {
        exe(cmd, args);
    }
}

// Returns a 1 if the command is "exit"
int parseCmd(char* input) {
    char cmdStr[strlen(input)];
    int a = 0;
    for(int i = 0; i < strlen(input); i++) {
        cmdStr[i] = *(input + a);
        a++;
    }
    history[historyIndex] = cmdStr;
    printf("parse cmd, history at hi: %s\n", history[historyIndex]);
    historyIndex = historyIndex + 1;
    printf("new hi: %d\n", historyIndex);
    
    // Get cmd
    char *cmd = strtok(input, " ");
    // Get args
    char *args = strtok(NULL, "");

    // If cmd is to exit, exit
    if(strcmp(cmd, "exit") == 0) {
        return 1;
    } else {
        // Send cmd and args 
        doCmd(cmd, args);
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
    printf("History of cmds: \n");
    for(int i = 0; i < numCmds; i++) {
        if(history[i] != NULL) {
            printf("%d %s\n", i, history[i]);
        }
    }
    return(EXIT_SUCCESS);
}
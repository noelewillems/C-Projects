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
    [x] executeProg - <program_name> <args>
    [x] changeDir - cd
    [x] listFiles - ls
    [x] showHistory - pwd
    [x] help - help
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
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#define INPUT_BUFF_SIZE 512
#define numCmds 100
// Global variable for history - initialized to 100, BUT pwd will truncate to 10.
// Array of pointers to char arrays. 
char* history[numCmds];
// Index tracker
int historyIndex = 0;

// Help section! 
// Source for fun colors: http://web.theurbanpenguin.com/adding-color-to-your-output-from-c/
void red () { printf("\033[1;31m"); }
void blue () { printf("\033[0;34m"); }
void yellow() { printf("\033[1;33m"); }
void cyan() { printf("\033[0;36m"); }
void magenta() { printf("\033[0;35m"); }
void green() { printf("\033[0;32m"); }
void reset() { printf("\033[0m"); }

void help() {
    magenta();
    printf("\n\n           ❀");
    cyan();
    printf("  ❀");
    green();
    printf("  ❀");
    cyan();
    printf("  Welcome to Noel's Shell! ");
    green();
    printf(" ❀");
    cyan();
    printf("  ❀");
    magenta();
    printf("  ❀\n");
    yellow();
    printf("\n                       Available Commands: \n\n");
    green();
    printf(" ► ");
    reset();
    printf("ls                          Displays list of files in current directory\n");
    green();
    printf(" ► ");
    reset();
    printf("cd <path>                   Change cwd to one specified by path. Entering \"cd ..\" will move up one directory\n");
    green();
    printf(" ► ");
    reset();
    printf("history                     Shows all past commands\n");
    green();
    printf(" ► ");
    reset();
    printf("!!                          Run previous command\n");
    green();
    printf(" ► ");
    reset();
    printf("! <n>                       Run nth command in history\n");
    green();
    printf(" ► ");
    reset();
    printf("<prog> <args>               Runs executable prog with args (if any)\n");
    green();
    printf(" ► ");
    reset();
    printf("alias <name> = <cmd>        Creates a new alias with called <name> that executes <cmd>\n");
    green();
    printf(" ► ");
    reset();
    printf("unalias <name>              Removes existing alias with the name <name>\n");
}
void alias() {

}

void runNthCmd(char* args) {
    printf("Running %s th command\n", args);
}

void runPrevCmd() {
    printf("Run previous cmd.\n");
    printf("Current history index: %d\n", historyIndex);
}
// history: show past commands.
void showHistory() {
    printf("History of cmds: \n");
    for(int i = 0; i < numCmds; i++) {
        if(history[i] != NULL) {
            printf("%d %s\n", i, history[i]);
        }
    }
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
    int status = fork();
    if(status < 0) {
        printf("Failed to execute %s!\n", cmd);
        exit(1);
    } else if(status == 0) {
        execv(cmd, &args);
    }
}

// Send cmds to their respective methods
void doCmd(char* cmd, char* args) {
    // Change dir
    if(strcmp(cmd, "cd") == 0) {
        changeDir(args);
    // List files
    } else if(strcmp(cmd, "ls") == 0) {
        int status;
        char *args[2];
        args[0] = "/bin/ls";
        args[1] = NULL;
        if (fork() == 0) { execv( args[0], args ); }
    // History
    } else if(strcmp(cmd, "history") == 0) {
        showHistory();
    // Show help
    } else if(strcmp(cmd, "help") == 0) {
        help();
    // Run previous command
    } else if(strcmp(cmd, "!!") == 0) {
        runPrevCmd();
    // Run nth command
    } else if(strcmp(cmd, "!") == 0) {
        runNthCmd(args);
    // If none of the above, executable
    } else {
        exe(cmd, args);
    }
}

// Returns a 1 if the command is "exit"
int parseCmd(char* input) {
    // Mallocing cmdStr
    char *cmdStr = (char*)malloc(strlen(input) * sizeof(char));
    // Filling in cmdStr with input
    int a = 0;
    for(int i = 0; i < strlen(input); i++) {
        cmdStr[i] = *(input + a);
        a++;
    }
    // Putting cmdStr in the array
    history[historyIndex] = cmdStr;
    // Moving down array
    historyIndex = historyIndex + 1;
    
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
        // TO DO: handle null input (blank line)
        printf("> ");
        fgets(input, INPUT_BUFF_SIZE, stdin);
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
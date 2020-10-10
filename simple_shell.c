/* 
Code created by Noel Willems: a simple shell program
Sources:
    Some concepts (no code taken from this site): https://brennan.io/2015/01/16/write-a-shell-in-c/
    Other sources listed in respective sections.

    ALL COMPONENTS:
    [x] main - main()
    [x] run - run()
    [x] parseCmd - parseCmd()
    [x] doCmd - do Cmd()
    [x] <program_name> <args> - executeProg
    [x] cd - change dir
    [x] ls - list files
    [x] history - show history
    [x] help - help
    [x] ! <n> - do nth cmd
    [x] !! - do prev cmd 
    [x] alias - show all aliases
    [] create new aliases
    [] remove aliases

    TO DO
    - create and remove aliases
    - Make history only 10 long
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
#define NUM_CMDS 100
#define CMD_LEN 60
#define NUM_ALIASES 20
#define NUM_ARGS 60

// Struct for aliases: the alias + the cmd string
struct aka {
    // Alias name
    char* aliasName;
    // Original cmd string
    char* cmdName;
} aka;

// Array of the aliases structs 
struct aka aliases[NUM_ALIASES];

// Array of pointers to char arrays of past commands. 
char* history[NUM_CMDS];
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
    printf("alias <name>='<cmd>'        Creates a new alias with called <name> that executes <cmd>\n");
    green();
    printf(" ► ");
    reset();
    printf("unalias <name>              Removes existing alias with the name <name>\n");
}
void createAlias(char* str) {
    printf("Received %s\n", str);
    char* alias = strtok(str, "=");
    char* cmd = strtok(NULL, "");
    printf("Alias \"%s\" should run cmd %s\n", alias, cmd);
}

// history: show past commands.
void showHistory() {
    printf("History of cmds: \n");
    for(int i = 0; i < NUM_CMDS; i++) {
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
        char* argsPtrs[NUM_ARGS];
        argsPtrs[0] = cmd;
        char* delim = " ";
        char* ar = strtok(args, delim); 
        int x = 1;
        while (ar != NULL) {
            argsPtrs[x] = ar;
            ar = strtok(NULL, delim);
            x = x + 1;
        }
        execvp(cmd, argsPtrs);
        exit(0);
    }
}

// Send cmds to their respective methods
void doCmd(char* cmd, char* args) {
    // Change dir
    if(strcmp(cmd, "cd") == 0) {
        changeDir(args);
    // Print working directory
    } else if (strcmp(cmd, "pwd") == 0){
        char s[100];
        // Get the current working directory path name
        getcwd(s, 100);
        printf("%s\n", s);
    // History
    } else if(strcmp(cmd, "history") == 0) {
        showHistory();
    // Show help
    } else if(strcmp(cmd, "help") == 0) {
        help();
    // Run previous command
    } else if(strcmp(cmd, "!!") == 0) {
        // Don't allow users to call !! first
        if(history[historyIndex-2] == NULL) {
            printf("\"!!\" cannot be the first command\n");
        // Avoid infinite loop if prev was !!
        } else if(strcmp(history[historyIndex-2], "!!") == 0) {
            printf("Sorry, cannot inifinitely call \"!!\"\n");
        } else {
            doCmd(history[historyIndex-2], args);
        }        
    // Run nth command
    } else if(strcmp(cmd, "!") == 0) {
        int i = atoi(args);
        // If you try to run nth command of a nonexistent n or a 0
        if(i >= historyIndex || i == 0) {
            printf("Command #%d doesn't exist\n", i);
        } else {
            doCmd(history[i], NULL);
        }
    // View all existing aliases
    } else if((strcmp(cmd, "alias") == 0) && args == NULL) {
        printf("List of aliases:\n");
        // Flag signifying array is empty
        int isEmpty = 1;
        for(int i = 0; i < NUM_ALIASES; i++) {
            if(aliases[i].aliasName != NULL) {
                isEmpty = 0;
                printf("Aliases at i: %s\n", aliases[i].aliasName);
            }
        }
        if(isEmpty) {
                printf("No existing aliases\n");
        }
    // Create an alias for a command
    } else if((strcmp(cmd, "alias") == 0) && args != NULL){
        createAlias(args);
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
    }
}

int main(int argc, char* argv[]) {
    run();
    return(EXIT_SUCCESS);
}
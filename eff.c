#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024

void execute_command(char* command) {
    // Tokenize the input command
    char* args[1024];
    int arg_count = 0;
    char* token = strtok(command, " \t\n");
    
    while (token != NULL) {
        args[arg_count] = token;
        arg_count++;
        token = strtok(NULL, " \t\n");
    }
    args[arg_count] = NULL; // Null-terminate the argument list
    
    // Handle built-in commands
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0) {
        if (arg_count > 1) {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        else {
            fprintf(stderr, "cd: missing argument\n");
        }
        return;
    }
    
    // Fork a child process to execute the command
    pid_t pid = fork();
    if (pid == 0) { // Child process
        // Input and Output Redirection
        int input_fd = -1;
        int output_fd = -1;
        
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "<") == 0) {
                if (args[i + 1] != NULL) {
                    input_fd = open(args[i + 1], O_RDONLY);
                    if (input_fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                    args[i] = NULL;
                }
                else {
                    fprintf(stderr, "Input file missing after '<'\n");
                    exit(1);
                }
            }
            else if (strcmp(args[i], ">") == 0) {
                if (args[i + 1] != NULL) {
                    output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (output_fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                    args[i] = NULL;
                }
                else {
                    fprintf(stderr, "Output file missing after '>'\n");
                    exit(1);
                }
            }
        }
        
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }
    else if (pid > 0) { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
    else {
        perror("fork");
        exit(1);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    
    while (1) {
        printf("osh> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        if (strlen(input) > 1) {
            input[strlen(input) - 1] = '\0'; // Remove newline character
            execute_command(input);
        }
    }
    
    return 0;
}

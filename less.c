#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 256

int main() {
    char input[MAX_CMD_LENGTH];
    bool finished = false;

    while (!finished) {
        printf("osh > ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "No command entered\n");
            exit(1);
        }

        input[strlen(input) - 1] = '\0'; // Remove the newline character
        
        if (strncmp(input, "exit", 4) == 0) {
            finished = true;
        } else {
            pid_t pid = fork();

            if (pid < 0) {
                fprintf(stderr, "Fork() failed\n");
            } else if (pid == 0) {
                // Child process
                if (system(input) < 0) {
                    fprintf(stderr, "Command execution failed\n");
                    exit(1);
                }
            } else {
                // Parent process
                int status = 0;
                while (wait(&status) != pid) {}
            }
        }
    }

    printf("\t\t...exiting\n");
    return 0;
}

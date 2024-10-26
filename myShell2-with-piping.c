#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX 1024
#define MAX_COMMANDS 10

void parse(char *line, char **argv)
{
    while (*line != '\0') {
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';
        *argv++ = line;
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n')
            line++;
    }
    *argv = '\0';
}

void execute(char **argv)
{
    int pipe_count = 0;
    char *commands[MAX_COMMANDS][64];
    int cmd_index = 0;
    int arg_index = 0;

    // Split commands by pipe
    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "|") == 0) {
            commands[cmd_index][arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
            pipe_count++;
        } else {
            commands[cmd_index][arg_index++] = argv[i];
        }
    }
    commands[cmd_index][arg_index] = NULL;
    cmd_index++;

    int n = cmd_index; // Number of commands
    
    fprintf(stderr, "Debug: Number of commands: %d\n", n);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "Debug: Command %d: ", i);
        for (int j = 0; commands[i][j] != NULL; j++) {
            fprintf(stderr, "%s ", commands[i][j]);
        }
        fprintf(stderr, "\n");
    }

    int pipe_arr[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    // Create pipes
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipe_arr[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // Fork processes and set up pipes
    for (int i = 0; i < n; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            exit(1);
        }
        
        if (pids[i] == 0) { // Child process
            // Set up stdin for all except first process
            if (i > 0) {
                if (dup2(pipe_arr[i-1][0], STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(1);
                }
            }
            
            // Set up stdout for all except last process
            if (i < n - 1) {
                if (dup2(pipe_arr[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(1);
                }
            }

            // Close all pipe FDs in child
            for (int j = 0; j < n - 1; j++) {
                close(pipe_arr[j][0]);
                close(pipe_arr[j][1]);
            }

            // Execute the command
            fprintf(stderr, "Debug: Executing command: %s\n", commands[i][0]);
            if (execvp(commands[i][0], commands[i]) < 0) {
                fprintf(stderr, "execvp error for command %s: %s\n", commands[i][0], strerror(errno));
                exit(1);
            }
        }
    }

    // Close all pipe FDs in parent
    for (int i = 0; i < n - 1; i++) {
        close(pipe_arr[i][0]);
        close(pipe_arr[i][1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < n; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            fprintf(stderr, "Debug: Child process %d exited with status %d\n", i, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Debug: Child process %d killed by signal %d\n", i, WTERMSIG(status));
        }
    }
}

int main(void)
{
    char line[MAX];
    char *argv[64];

    while (1) {
        printf("Shell -> ");
        fflush(stdout);
        if (fgets(line, MAX, stdin) != 0) {
            line[strcspn(line, "\n")] = '\0';

            if (strcmp(line, "exit") == 0) {
                printf("Exiting shell...\n");
                exit(0);
            }

            parse(line, argv);
            execute(argv);
        }
    }
    return 0;
}
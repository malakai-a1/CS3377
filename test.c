#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX 1024

void parse(char *line, char **argv);
void execute(char **argv);

int main(int argc, char *argv[]) {
    // Existing code for file redirection
    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "<") == 0) {
            int fd = open(argv[i+1], O_RDONLY);  // Open file
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);  // Redirect stdin to file
            close(fd);
            break;
        }
    }

    // Process commands separated by semicolons
    for (int i = 0; argv[i] != NULL; i++) {
        if (strchr(argv[i], ';') != NULL) {
            char *token = strtok(argv[i], ";");
            char *commands[MAX];
            int j = 0;
            while (token != NULL) {
                commands[j++] = token;
                token = strtok(NULL, ";");
            }
            commands[j] = NULL;
            for (int k = 0; k < j; k++) {
                parse(commands[k], argv);
                execute(argv);
            }
        } else {
            parse(argv[i], argv);
            execute(argv);
        }
    }

    return 0;
}

void parse(char *line, char **argv) {
    while (*line != '\0') {
        while (*line == ' ' || *line == '\t' || *line == '\n') {
            *line++ = '\0';
        }
        *argv++ = line;
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') {
            line++;
        }
    }
    *argv = '\0';
}

void execute(char **argv) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        if (execvp(*argv, argv) < 0) {
            perror("execvp");
            exit(1);
        }
    } else {
        while (wait(&status) != pid);
    }
}
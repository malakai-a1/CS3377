#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 1024

void parse(char *line, char **argv) {
    while (*line != '\0') {
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';
        *argv++ = line;
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n' && *line != ';')
            line++;
        if (*line == ';') {
            *line++ = '\0';
            break;
        }
    }
    *argv = '\0';
}

void execute(char **argv) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    } else if (pid == 0) {
        if (execvp(*argv, argv) < 0) {
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    } else {
        while (wait(&status) != pid) {}
    }
}

void handle_redirection(char **argv) {
    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "<") == 0) {
            int fd = open(argv[i+1], O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            argv[i] = NULL;
        } else if (strcmp(argv[i], ">") == 0) {
            int fd = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            argv[i] = NULL;
        }
    }
}

void handle_pipe(char **argv) {
    int pipefd[2];
    pid_t pid1, pid2;
    char *cmd1[MAX], *cmd2[MAX];
    int i, j;

    for (i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "|") == 0) {
            argv[i] = NULL;
            break;
        }
        cmd1[i] = argv[i];
    }
    cmd1[i] = NULL;
    for (j = 0, i = i + 1; argv[i] != NULL; i++, j++) {
        cmd2[j] = argv[i];
    }
    cmd2[j] = NULL;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    if ((pid1 = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(cmd1[0], cmd1);
        perror("execvp");
        exit(1);
    } else {
        if ((pid2 = fork()) == -1) {
            perror("fork");
            exit(1);
        }

        if (pid2 == 0) {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(cmd2[0], cmd2);
            perror("execvp");
            exit(1);
        } else {
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
}

int main(void) {
    char line[MAX];
    char *argv[MAX];
    char *commands[MAX];
    int i;

    while (1) {
        printf("Shell -> ");
        if (fgets(line, MAX, stdin) != NULL) {
            line[strcspn(line, "\n")] = '\0';

            if (strcmp(line, "exit") == 0) {
                printf("Exiting shell...\n");
                exit(0);
            }

          if (strchr(line, '|')) {
               parse(line, argv);
               handle_pipe(argv);
          } 
          if (strchr(line, ';')) {
               char *cmd = strtok(line, ";");
               while (cmd != NULL) {
                    parse(cmd, argv);
                    handle_redirection(argv);
                    execute(argv);
                    cmd = strtok(NULL, ";");
               }
          } else {
               parse(line, argv);
               handle_redirection(argv);
               execute(argv);
          }
        }
    }
    return 0;
}
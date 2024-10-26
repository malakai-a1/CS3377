// Name: Malakai Ashenafi
// NETID: mga190001
// CS 3377.001

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX 1024
#define MAX_COMMANDS 10

volatile sig_atomic_t alarm_flag = 0;

void mysig_handler(int signo){
     printf("\n ** This is the message from week09 lab2 - Signal Handler! ** \n");
     printf("Exiting shell...\n");
     exit(0);
}

void alarm_handler(int signo){
     printf("\nAlarm time expired. Terminating the shell.\n");
     printf("Exiting due to alarm...\n");
     alarm_flag = 1;
     exit(0);
     
}

void parse(char *line, char **argv)
{
     while (*line != '\0'){
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
     int i = 0;
     int j = 0;


     // Split commands by pipe
     for (i = 0; argv[i] != NULL; i++){
          if (strcmp(argv[i], "|") == 0){
               commands[cmd_index][arg_index] = NULL;
               cmd_index++;
               arg_index = 0;
               pipe_count++;
          }
          else{
               commands[cmd_index][arg_index++] = argv[i];
          }
     }
     commands[cmd_index][arg_index] = NULL;
     cmd_index++;

     int n = cmd_index; // Number of commands

     int pipe_arr[MAX_COMMANDS - 1][2];
     pid_t pids[MAX_COMMANDS];

     // Create pipes
     for (i = 0; i < n - 1; i++){
          if (pipe(pipe_arr[i]) == -1){
               perror("pipe");
               exit(1);
          }
     }

     // Fork processes and set up pipes
     for (i = 0; i < n; i++){
          pids[i] = fork();
          if (pids[i] == -1){
               perror("fork");
               exit(1);
          }

          if (pids[i] == 0){       // Child process
               // Set up stdin for all except first process
               if (i > 0){
                    if (dup2(pipe_arr[i - 1][0], STDIN_FILENO) == -1){
                         perror("dup2 stdin");
                         exit(1);
                    }
               }

               // Set up stdout for all except last process
               if (i < n - 1){
                    if (dup2(pipe_arr[i][1], STDOUT_FILENO) == -1){
                         perror("dup2 stdout");
                         exit(1);
                    }
               }

               // Close all pipe FDs in child
               for (j = 0; j < n - 1; j++){
                    close(pipe_arr[j][0]);
                    close(pipe_arr[j][1]);
               }

               // Execute the command
               if (execvp(commands[i][0], commands[i]) < 0){
                    fprintf(stderr, "execvp error for command %s: %s\n", commands[i][0], strerror(errno));
                    exit(1);
               }
          }
     }

     
     for (i = 0; i < n - 1; i++){
          close(pipe_arr[i][0]);                  // Close read end of pipe
          close(pipe_arr[i][1]);                  // Close write end of pipe  
     }

     
     for (i = 0; i < n; i++){
          waitpid(pids[i], NULL, 0);              // Wait for all child processes to finish
     }
}

int main(void)
{
     char line[MAX];
     char *argv[64];

     signal(SIGINT, mysig_handler);               // Set up signal handler for SIGINT
     signal(SIGALRM, alarm_handler);              // Set up signal handler for SIGALRM

     
     while (1){                                   // Repeat until done
          printf("Shell -> ");
          fflush(stdout);

          if (fgets(line, MAX, stdin) == NULL){
               if (alarm_flag){                     // checks if the alarm flag is set to 1
                    printf("Exiting due to alarm...\n");
                    exit(0);
               }
               continue;
          }

          line[strcspn(line, "\n")] = '\0';


          if (strcmp(line, "exit") == 0){         // checks if the command is "exit"
               printf("Exiting shell...\n");
               exit(0);                           // exits the shell
          }

          parse(line, argv);                      // parses the command

          if (strcmp(argv[0], "alarm") == 0){     // checks if the command is "alarm"
               if (argv[1] != NULL){              // checks if the alarm time is provided
                    int seconds = atoi(argv[1]);  // converts the alarm time to an integer
                    if (seconds > 0){             // checks if the alarm time is valid, sets the alarm to the given time.
                    
                         printf("Alarm set for %d seconds.\n", seconds);
                         alarm(seconds);
                    }
                    else if (seconds == 0){        // checks if the alarm time is 0, if so turns off the alarm
                         printf("Alarm turned off.\n");
                         alarm(0);
                    }
                    else{                          // if the alarm time is invalid, prints an error message
                         printf("Invalid alarm time.\n");
                    }
               }
               else{
                    printf("Usage: alarm <seconds>\n");  // if the alarm time is not provided, prints the usage message
               }
          }
          else { execute(argv); } // executes the command
     }

     return 0;
}
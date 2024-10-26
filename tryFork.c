#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void run_case(int case_num) {
    pid_t pid, pid2;

    switch (case_num) {
        case 1:
            printf("\n\nCase #1: Single fork()\n");
            pid = fork();
            if (pid == 0) {
                printf("[Child] Process ID: %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            } else {
                printf("[Parent] Process ID: %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            }
            printf("End of case #1\n");
            break;

        case 2:
            printf("\n\nCase #2: Two fork() calls\n");
            pid = fork();
            if (pid == 0) {
                // First child
                printf("[Child 1] Process ID: %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            } else {
                // Parent continues
                pid2 = fork();
                if (pid2 == 0) {
                    // Second child
                    printf("[Child 2] Process ID: %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
                } else {
                    // Parent
                    printf("[Parent] Process ID: %ld\n", (long)getpid());
                    printf("End of case #2\n");
                }
            }
            break;

        case 3:
            printf("\n\nCase #3: Three fork() calls\n");
            fork();  // Creates first child
            fork();  // Creates two more children
            fork();  // Creates four more children
            printf("Process ID: %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            printf("End of case #3\n");
            break;

        case 4:
            printf("\n\nCase #4: Complex conditional fork()s\n");
            if ((pid = fork()) && (pid2 = fork())) {
                fork();
                printf("Process ID after first fork(): %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            }
            if ((pid = fork()) && (pid2 = fork())) {
                fork();
                printf("Process ID after second fork(): %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            }
            if ((pid = fork()) && (pid2 = fork())) {
                fork();
                printf("Process ID after third fork(): %ld (Parent ID: %ld)\n", (long)getpid(), (long)getppid());
            }
            printf("End of case #4\n");
            break;

        case 5:
            printf("\n\nCase #5: Fork in loop\n");
            for (int i = 1; i < 5; i++) {
                pid = fork();
                if (pid == 0) {
                    printf("[Child %d] Process ID: %ld (Parent ID: %ld)\n", i, (long)getpid(), (long)getppid());
                }
            }
            printf("[Parent] Process ID: %ld\n", (long)getpid());
            printf("End of case #5\n");
            break;

        default:
            printf("Invalid case number.\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./tryFork <case_number>\n");
        exit(1);
    }

    int case_num = atoi(argv[1]);
    run_case(case_num);

    return 0;
}

/* sample shell to handle two commands with pipe: ls | wc -l */

#include <stdlib.h>
#include <stdio.h>

char *cmd1[] = { "/bin/ls", 0 };

char *cmd2a[] = { "/bin/ls", 0 };
char *cmd2b[] = { "/bin/wc", "-l", 0 };

char *cmd3a[] = { "/bin/ls", 0 };
char *cmd3b[] = { "/bin/grep", "c$", 0 };
char *cmd3c[] = { "/bin/wc", "-l", 0 };


void run1com();
void run2com();

int main(int argc, char **argv)
{
	int pid, status;
	pid = fork();

	if (pid == 0) {
		run1com();
		exit(0);

	} else if (pid > 0) {
		while ((pid = wait(&status)) != -1)
		 fprintf(stderr, "process %d exits with %d\n", 
				pid, WEXITSTATUS(status));
	} else {
		perror("fork");
		exit(1);
	}

	pid = fork();
	if (pid == 0) {
		run2com();
		exit(0);

	} else if (pid > 0) {
		while ((pid = wait(&status)) != -1)
		 fprintf(stderr, "process %d exits with %d\n", 
				pid, WEXITSTATUS(status));

	} else {
		perror("fork");
		exit(1);
	}

	exit(0);

}

void run1com()
{
	execvp(cmd1[0], cmd1);
}



void run2com()
{
	int pfd[2];
	pipe(pfd);

	int pid;
	pid = fork();

	if (pid > 0) { 				// parent runs cmd2a
		dup2(pfd[1], 1);
		close(pfd[0]);	
		execvp(cmd2a[0], cmd1);

		perror(cmd2a[0]);

	} else if (pid ==0) {			// child runs cmd2b
		dup2(pfd[0], 0);
		close(pfd[1]);	
		execvp(cmd2b[0], cmd2);

		perror(cmd2b[0]);

	} else {
		perror("fork");
		exit(1);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char line[256];							// max amount of characters on one line.
	char prompt[] = "myshell % "; 					// shell prompt

	printf("%s", prompt); 						// displays shell prompt.
	while( fgets(line, sizeof line, stdin) != NULL ) 		// trys to get input
	{
		line[strlen(line)-1] = '\0'; 				// fgets leaves '\n' in input buffer. ditch it.
		if(strcmp(line, "exit") == 0){
			printf("Shell terminated.\n\n\n"); 		// shell exit prompt.
			exit(0); 					// exits shell
		}

		else if(strcmp(line, "listall") == 0){
			strcpy(line, "ls -la"); 			// shows all files (hidden and regular)

		}

		else if(strcmp(line, "showme") == 0) {
			snprintf(line, sizeof(line), "w | grep mga"); 	// showme input is replaced with "w | grep mga". displays my info.
		}
		
		system(line); 						// system call to execute the input command

		printf("%s", prompt); 					// displays the shell prompt 

	}
	return 0;
}






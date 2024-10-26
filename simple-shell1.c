// shell1.c
// simple shell example using system call for command processing
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char line[256];
    char prompt[] = "sh1 % ";

    /* spit out the prompt */
    printf("%s", prompt );

    /* Try getting input. If error or EOF, exit */
    while( fgets(line, sizeof line, stdin) != NULL )
    {
	/* fgets leaves '\n' in input buffer. ditch it */
	line[strlen(line)-1] = '\0';
	system( line );       // easy shell to be worked.
	printf("%s", prompt );
    }
    return 0;
}
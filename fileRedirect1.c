// Code sample - file redirection  
// C program to handle file redirection to set input.txt
// to be standard input: fileRedir("<", "input.txt");
//

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

 
void fileRedir(char * redirection, char* file){
    int fdout;
    if(!strcmp(redirection, "<")){
        if ((fdout = open(file, O_CREAT | O_RDONLY, 0700)) < 0) {
            perror(file); /* open failed */
        }
        dup2(fdout, 0);
        }
    else if(!strcmp(redirection, ">")){
        if ((fdout = open(file, O_CREAT | O_WRONLY, 0700)) < 0) {
                perror(file); /* open failed */
        }
        dup2(fdout, 1);
    }
}

/* This still very simple ls with lstat and stat calls
   for the file information in a directory.

 */

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char **argv) {

	DIR *dp;
	struct dirent *dirp;
	int regular_files = 0;
    int directories = 0;
    int symlinks = 0;
	int blocks = 0;
	int fifos = 0;
	int sockets = 0;
	int chars = 0;
	int unknowns = 0;
    int total_entries = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s dir_name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((dp = opendir(argv[1])) == NULL ) {
		fprintf(stderr, "can't open '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (chdir(argv[1]) == -1) {
		fprintf(stderr, "can't chdir to '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Reading the directory: %s\n", argv[1]);

	while ((dirp = readdir(dp)) != NULL ) {
		struct stat sb;
		if (stat(dirp->d_name, &sb) == -1) {
			fprintf(stderr, "Can't stat %s: %s\n", dirp->d_name,
						strerror(errno));

			if (lstat(dirp->d_name, &sb) == -1) {
				fprintf(stderr,"Can't stat %s: %s\n", dirp->d_name,
						strerror(errno));
				continue;
			}
		}

		total_entries++;

		if (S_ISREG(sb.st_mode))
			regular_files++;
		else if (S_ISDIR(sb.st_mode))
			directories++;
		else if (S_ISCHR(sb.st_mode))
			chars++;
		else if (S_ISBLK(sb.st_mode))
			blocks++;
		else if (S_ISFIFO(sb.st_mode))
			fifos++;
		else if (S_ISLNK(sb.st_mode))
			symlinks++;
		else if (S_ISSOCK(sb.st_mode))
			sockets++;
		else
			unknowns++;



		printf("%s: ", dirp->d_name);
		if (S_ISREG(sb.st_mode))
			printf("regular file");
		else if (S_ISDIR(sb.st_mode))
			printf("directory");
		else if (S_ISCHR(sb.st_mode))
			printf("character special");
		else if (S_ISBLK(sb.st_mode))
			printf("block special");
		else if (S_ISFIFO(sb.st_mode))
			printf("FIFO");
		else if (S_ISLNK(sb.st_mode))
			printf("symbolic link");
		else if (S_ISSOCK(sb.st_mode))
			printf("socket");
		else
			printf("unknown");

		printf(" -- with stat\n");

		if (lstat(dirp->d_name, &sb) == -1) {
			fprintf(stderr,"not for stat %s: %s\n", dirp->d_name,
						strerror(errno));
			continue;
		}

		printf("%s: ", dirp->d_name);
		if (S_ISREG(sb.st_mode))
			printf("regular file");
		else if (S_ISDIR(sb.st_mode))
			printf("directory");
		else if (S_ISCHR(sb.st_mode))
			printf("character special");
		else if (S_ISBLK(sb.st_mode))
			printf("block special");
		else if (S_ISFIFO(sb.st_mode))
			printf("FIFO");
		else if (S_ISLNK(sb.st_mode))
			printf("symbolic link");
		else if (S_ISSOCK(sb.st_mode))
			printf("socket");
		else
			printf("unknown");

		printf(" -- with lstat\n\n");
	}

	closedir(dp);

	printf("Count of Regular files : %d\n", regular_files);
	printf("Count of Directories: %d\n", directories);
	printf("Count of Symbolic links: %d\n", symlinks);
	printf("Count of Block special files: %d\n", blocks);
	printf("Count of Character special files: %d\n", chars);
	printf("Count of FIFOs: %d\n", fifos);
	printf("Count of Sockets: %d\n", sockets);
	printf("Count of Unknowns: %d\n", unknowns);
	printf("Total entries: %d\n", total_entries);


	exit(EXIT_SUCCESS);
}

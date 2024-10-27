#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define	MAX_PATH	256

void
JustDoIt(char *path)
{
	DIR				*dp;
	struct dirent	*dep;
	struct stat		statbuf;
	char			fullpath[MAX_PATH];

	//Open the directory specified by path
	if ((dp = opendir(path)) == NULL)  {
		perror("opendir");
		exit(0);
	}

	printf("\n%s:\n", path);	//Print the directory name
	//Print the names of all files and directories in the current directory
	while (dep = readdir(dp))  {
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)	//Skip the "." and ".." entries
			continue;
		printf("%s\n", dep->d_name);
	}

	rewinddir(dp);	//Rewind the directory stream to read entries again
	while (dep = readdir(dp))  {	//Recursively call JustDoIt on subdirectories
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)
			continue;
		//Construct the full path for the current entry
		strcpy(fullpath, path);
		strcat(fullpath, "/");
		strcat(fullpath, dep->d_name);
		if (lstat(fullpath, &statbuf) < 0)  {	//Get the file status using lstat
			perror("lstat");
			exit(1);
		}
		if (S_ISDIR(statbuf.st_mode))  {	//If the entry is a directory, call JustDoIt recursively
			JustDoIt(fullpath);
		}
	}

	closedir(dp);
}

main()
{
	JustDoIt(".");
}
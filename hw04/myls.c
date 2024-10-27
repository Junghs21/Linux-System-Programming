#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

main()
{
	DIR				*dp;
	struct dirent	*dep;

	//Open the current directory and get a directory pointer
	if ((dp = opendir(".")) == NULL)  {	//. is meaing of current directory
		perror("opendir");
		exit(0);
	}

	//Read each entry in the directory using readdir
	while (dep = readdir(dp))  {
		printf("%s\n", dep->d_name);	//Print the name of each entry in the directory
	}

	closedir(dp);
}
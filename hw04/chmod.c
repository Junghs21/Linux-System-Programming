#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
	//stat Structure to save file information
	struct stat	statbuf;

	//Get the current file status of "bar" and store it in statbuf
	if (stat("bar", &statbuf) < 0)  {
		perror("stat");
		exit(1);
	}

	//Change the permissions of "bar"
	//Remove group write permissions(S_IWGRP) and set the (S_ISUID) bit
	if (chmod("bar", (statbuf.st_mode & ~S_IWGRP) | S_ISUID) < 0)  {
		perror("chmod");
		exit(1);
	}

	//Change the permissions of "foo"
	//Read and write permissions for the owner(S_IRUSR | S_IWUSR)
	//Read permission for the group(S_IRGRP)
	//Read permission for others(S_IROTH)
	if (chmod("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)  {
		perror("chmod");
		exit(1);
	}
}
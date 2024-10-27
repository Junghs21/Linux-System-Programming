#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
	umask(0);	//Set the file creation mask to 0
	//Create a file named "bar" with read/write permissions for user, group and others
	if (creat("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}

	//Set a new umask that restricts group read/write and others read/write permissions
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	//Create a file named "foo" with the same permissions specified, but the umask will modify the final permissions
	if (creat("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}
}
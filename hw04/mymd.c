#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main(int argc, char *argv[])
{
	//Check if the number of arguments is exactly 2(program name and directory name)
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	/* 755 permission */
	//-> Owner can read/write/execute, group and others can read/execute 
	if (mkdir(argv[1], 0755) < 0)  {	//Attempt to create a directory with 755 permissions
		perror("mkdir");
		exit(1);
	}
}
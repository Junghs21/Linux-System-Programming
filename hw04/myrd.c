#include <stdio.h>
#include <unistd.h>

main(int argc, char *argv[])
{
	//Check if the number of arguments is exactly 2(program name and directory name)
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	//Attempt to remove the directory specified in argv[1]
	if (rmdir(argv[1]) < 0)  {
		perror("rmdir");
		exit(1);
	}
}
#include <stdio.h>

main(int argc, char *argv[])
{
	//Check if the number of arguments is exactly 3(program name, old filename, new filename)
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s old new\n", argv[0]);
		exit(1);
	}

	//Attempt to rename the file from old name(argv[1]) to new name(argv[2])
	if (rename(argv[1], argv[2] < 0))  {
		perror("rename");
		exit(1);
	}
}
#include <stdio.h>
#include <unistd.h>

main(int argc, char *argv[])
{
	//Check if the number of arguments is exactly 3(program name, source file, destination link)
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destinaion\n", argv[0]);
		exit(1);
	}

	//Create a symbolic link from the source file to the destination file
	if (symlink(argv[1], argv[2]) < 0)  {
		perror("link");
		exit(1);
	}
}
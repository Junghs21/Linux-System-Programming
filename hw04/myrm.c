#include <stdio.h>

main(int argc, char *argv[])
{
	int		i;

	//Check if the number of arguments is at least 2(program name and at least one filename)
	if (argc == 1)  {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	//Loop through each filename provided as an argument and attempt to remove it
	for (i = 1 ; i < argc ; i++)  {
		//Attempt to remove the file
		if (remove(argv[i] < 0))  {	//if it fails, print an error message and exit
			perror("remove");
			exit(1);
		}
	}
}
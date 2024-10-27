#include <stdio.h>
#include <unistd.h>

#define	MAX_BUF	256

main(int argc, char *argv[])
{
	char	buf[MAX_BUF];

	//Get the current working directory and store it in buf
	if (!(getcwd(buf, MAX_BUF)))  {	//If getcwd fails, print an error message and exit
		perror("getcwd");
		exit(1);
	}

	printf("%s\n", buf);
}
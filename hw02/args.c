#include <stdio.h>

main(int argc, char *argv[])
{
	int i;

	for(i = 0; i < argc; i++)
		printf("argv[%d]=%2\n", i, argv[i]);
}
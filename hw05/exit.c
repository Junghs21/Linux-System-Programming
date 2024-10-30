#include <stdio.h>
#include <stdlib.h>

void
myexit1()
{
	printf("first exit handler\n");
}

void myexit2()
{
	printf("second exit handler\n");
}

main()
{
	/*Add myexit2 to exit functions */
	if(atexit(myexit2) != 0) {	//Register myexit2 as an exit handler
		perror("atexit");
		exit(1);
	}
	
	if (atexit(myexit1) != 0) {
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0) {
		perror("atexit");
		exit(1);
	}

	printf("main is done\n");	//When this program exits, myexit1 and myexit2 functions will be called in the reverse order of their registration

	//Output will be
	/*
	 * main is done
	 * first exit handler
	 * first exit handler
	 * second exit handler
	 */
}
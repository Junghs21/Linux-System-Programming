#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
	int pid;

	//Attempt to create a new process using fork()
	if((pid == fork()) < 0) {	//Use fork() and check error
		perror("fork");
		exit(1);
	}
	else if (pid == 0) {
		/* child */
		printf("I am %d. My parent is %d.\n", getpid(), getppid());
	}
	else {
		/* parent */
		printf("I am %d. My child is %d.\n", getpid(), pid);
	}
}
#include <stdio.h>
#include "synclib.h"

#define	NLOOPS	5


main()
{
	int		i;
	pid_t	pid;

	//Create syncrhonization resources
	TELL_WAIT();	//Initialize the synchronization mechanisms

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid > 0)  {
		for (i = 0 ; i < NLOOPS ; i++)  {
			TELL_CHILD();	//Tell the child process it's okay to proceed
			printf("Parent: Tell to child\n");
			printf("Parent: Wait for child to tell\n");
			WAIT_CHILD();	//Wait for the child to signal back
		}
	}
	else  {
		for (i = 0 ; i < NLOOPS ; i++)  {
			printf("Child: Wait for parent to tell\n");
			WAIT_PARENT();	//Wait for the parent to signal it's okay to proceed
			TELL_PARENT();	//Tell the parent process it's okay to proceed
			printf("Child: Tell to parent\n");
		}
	}
}
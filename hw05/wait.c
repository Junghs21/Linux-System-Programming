#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

main()
{
	pid_t pid;
	int status;

	if((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if(pid == 0) {	//Child process code
		printf("I'm a child\n");
		sleep(2);
	}
	else {	//Parent process code
		/* wait */
		wait(&status);	//Wait for the child process to finish and store its exit status
		printf("A child killed with %d status\n", status);	//Print the child's exit status
	}
}
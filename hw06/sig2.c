#include <stdio.h>
#include <signal.h>

void
SigUsrHandler(int signo)
{
	if (signo == SIGUSR1)  {
		printf("Received a SIGUSR1 signal\n");
	}
	else if (signo == SIGUSR2)  {
		printf("Received a SIGUSR2 signal\n");
	}
	else  {
		printf("Received unknown signal\n");
		printf("Terminate this process\n");
		exit(0);
	}
}

main()
{
	/* SIGUSR1 signal handler: SigUsrHandler */
	if (signal(SIGUSR1, SigUsrHandler) == SIG_ERR)  {	//Set the "SIGUSR1" signal handler to "SigUsrHandler" function
		perror("signal");	//signal returns "SIG_ERR" when an error occurs
		exit(1);
	}

	if (signal(SIGUSR2, SigUsrHandler) == SIG_ERR)  {
		perror("signal");
		exit(1);
	}

	for ( ; ; )
		pause();
}
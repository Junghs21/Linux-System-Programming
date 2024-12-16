#include <stdio.h>
#include <signal.h>

void
SigIntHandler(int signo)
{
	printf("Received a SIGINT signal\n");
	printf("Terminate this process\n");

	exit(0);
}

main()
{
	/* SIGINT signal handler: SigIntHandler */
	/* signal */
	signal(SIGINT, SigIntHandler);	//When the "SIGINT" signal comes in, it is processed with the "SigIntHandler" function

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause();
}
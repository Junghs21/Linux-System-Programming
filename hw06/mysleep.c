#include <stdio.h>
#include <signal.h>

void
SigAlarmHandler(int signo)
{
	/* nothing to do, just return to wake up the pause */
	return;
}

unsigned int
mysleep(unsigned int nsecs)
{
	/* SIGALRM signal handler: SigAlarmHandler */
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {	//Set the "SIGALRM" signal handler to "SigAlarmHandler"
		return nsecs;	//When signal occurs error, signal function returns "SIG_ERR" and Returns nsecs as this condition statement is executed
	}

	/* alarm */
	alarm(nsecs);	//Schedule an alarm after nsecs seconds

	pause();

	return alarm(0);
}

main()
{
	printf("Wait for 5 seconds...\n");

	mysleep(5);
}
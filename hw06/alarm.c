#include <stdio.h>
#include <signal.h>

static unsigned int	AlarmSecs;


//Signal handler for "SIGALRM"
void
SigAlarmHandler(int signo)
{
	//Set up the signal handler again for periodic alarm
	if(signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {
		perror("signal");
		exit(1);
	}

	/* alarm */
	alarm(AlarmSecs);

	/* Do something */
	printf(".");
	fflush(stdout);	//Ensure the dot is printed immediately

	return;
}

//Function to set a periodic alarm every nsecs seconds
int
SetPeriodicAlarm(unsigned int nsecs)
{
	//Set up the signal handler for "SIGALRM"
	if(signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {
		return -1;
	}

	AlarmSecs = nsecs;

	/* alarm */
	alarm(nsecs);

	return 0;
}

main()
{
	printf("Doing something every one seconds\n");

	SetPeriodicAlarm(1);

	for ( ; ; )
		pause();
}
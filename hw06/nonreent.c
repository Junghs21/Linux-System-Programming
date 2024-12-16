#include <stdio.h>
#include <signal.h>
#include <pwd.h>


//Signal handler for "SIGALRM"
void
MyAlarmHandler(int signo)
{
	struct passwd	*rootptr;

	//Reset the "SIGALRM" handler to "MyAlarmHandler for subsequent alarms
	signal(SIGALRM, MyAlarmHandler);
	//Schedule the next alarm in 1 second
	alarm(1);

	printf("in signal handler\n");

	//Retrieve the password file entry for the user "root"
	if ((rootptr = getpwnam("root")) == NULL)  {
		perror("getpwnam");
		exit(1);
	}

	return;
}

main()
{
	struct passwd	*ptr;

	//Set up "SIGALRM" handler to "MyAlarmHandler" function
	signal(SIGALRM, MyAlarmHandler);
	//Schedule the first alarm after 1 second
	alarm(1);

	//Infinite loop to continuously check password entry for user "cjs"
	for ( ; ; )  {
		if ((ptr = getpwnam("cjs")) == NULL)  {	//Retrieve the password file entry for the user "cjs"
			perror("getpwnam");
			exit(1);
		}

		//Verify if the returned username is indeed "cjs"
		if (strcmp(ptr->pw_name, "cjs") != 0)  {
			printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
			exit(0);
		}
	}
}
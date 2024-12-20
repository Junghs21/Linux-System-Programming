#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

//Custom function to implement a precise sleep using select()
void
myusleep(int secs, int usecs)
{
	struct timeval	tm;

	//Set the time interval for sleep
	tm.tv_sec = secs;	//Seconds
	tm.tv_usec = usecs;	//Microseconds

	//Use select() to wait for the specified time
	if (select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &tm) < 0)  {
		perror("select");
		return;
	}
}

main()
{
	printf("Sleep for 1.5 seconds.....\n");
	myusleep(1, 500000);	//Call the custom sleep function to sleep for 1.5 seconds
}
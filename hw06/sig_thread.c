#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#define	THREAD_MAIN
/*
 * #define	THREAD_1
 * #define	THREAD_2
 * */


//Signal handler for "SIGINT" (Ctrl + C)
void
SigIntHandler(int signo)
{
	printf("Received a SIGINT signal by thread %d\n", pthread_self());	//Print the thread ID that received "SIGINT"
	printf("Terminate this process\n");

	exit(0);
}

//Function for the first thread
void
Thread1(void *dummy)
{
#ifdef	THREAD_1
	signal(SIGINT, SigIntHandler);	//Set up "SIGINT" signal handler for Thread1 if THREAD_! is defined
#endif

	while (1)
		;
}

//Function for the second thread
void
Thread2(void *dummy)
{
#ifdef	THREAD_2
	signal(SIGINT, SigIntHandler);	//Set up "SIGINT" signal handler for Thread2 if THREAD_@ is defined
#endif

	while (1)
		;
}

main()
{

	pthread_t	tid1, tid2;

	//Create the first thread, running Thread1 function
	if (pthread_create(&tid1, NULL, (void *)Thread1, NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}
	//Create the second thread, running Thread2 function
	if (pthread_create(&tid2, NULL, (void *)Thread2, NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}
	printf("Create two threads: tid1=%d, tid2=%d\n", tid1, tid2);
	printf("Main thread: tid=%d\n", pthread_self());

#ifdef	THREAD_MAIN
	signal(SIGINT, SigIntHandler);	//Set up "SIGINT" signal handler for the main thread if THREAD_MAIN is defined
#endif

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause();	//Wait for signals indenfinitely
}
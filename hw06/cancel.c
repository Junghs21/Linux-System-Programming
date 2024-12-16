#include <stdio.h>
#include <signal.h>
#include <pthread.h>

pthread_t	ThreadId[2];

//Signal handler for "SIGINT"
void
SigIntHandler(int signo)
{
	int		i;

	printf("Received a SIGINT signal by thread %d\n", pthread_self());
	printf("Terminate other threads: tid=%d, %d\n", ThreadId[0], ThreadId[1]);

	//Cancel execution of all threads
	for (i = 0 ; i < 2 ; i++)  {
		/* Cancel execution of all threads */
		if (pthread_cancel(ThreadId[i]))  {	//Request cancellation of each thread
			perror("pthread_cancel");	//Print error if cancellation fails
			exit(1);
		}
	}
	for (i = 0 ; i < 2 ; i++)  {	//Wait for all threads to terminate
		if (pthread_join(ThreadId[i], NULL))  {	//Wait for each thread to finish
			perror("pthread_join");
			exit(1);
		}
	}
	printf("Threads terminated: tid=%d, %d\n", ThreadId[0], ThreadId[1]);

	exit(0);
}

//Function executed by the first thread
void
Thread1(void *dummy)
{
	printf("Thread %d created.....\n", pthread_self());

	//Enable thread cancellation
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
		perror("pthread_setcancelstate");
		pthread_exit(NULL);
	}
	//Set cancellation type to asynchronous
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
		perror("pthread_setcanceltype");
		pthread_exit(NULL);
	}

	while (1)
		;
}

//Function executed by the second thread
void
Thread2(void *dummy)
{
	printf("Thread %d created.....\n", pthread_self());

	//Enable thread cancellation
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
		perror("pthread_setcancelstate");
		pthread_exit(NULL);
	}
	//Set cancellation type to asynchronous
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
		perror("pthread_setcanceltype");
		pthread_exit(NULL);
	}

	while (1)
		;
}

main()
{
	//Create the first thread
	if (pthread_create(&ThreadId[0], NULL, (void *)Thread1, NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}
	//Create the second thread
	if (pthread_create(&ThreadId[1], NULL, (void *)Thread2, NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	//Set up "SIGINT" handler to "SigIntHandler" function
	signal(SIGINT, SigIntHandler);

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause();
}
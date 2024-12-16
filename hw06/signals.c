#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/**
 * [Function Name] : void SigChildHandler(int signo)
 * [Description]   : 
 * 	- This function is a signal handler for "SIGCHLD"
 * 	- Waits for a child process to terminate to prevent zombie process
 * [Input]	   :
 * 	int signo - Signal number of "SIGCHLD", indicating a child process has terminated
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Called automatically by the system when a "SIGCHLD" signal is received
 * [Calls]	   :
 * 	wait() - waits for a child process to terminate and prevents zombie process
 * [Given]	   :
 * 	- Assumes that the process has set up a signal handler for "SIGCHLD"
 * [Returns]	   :
 * 	Nothing
 */
void SigChildHandler(int signo) {	//Signal handler for "SIGCHLD"

	wait(NULL);	//Wait for a child process to terminate

	printf("Received a SIGCHLD.\n");
}

/**
 * [Program Name] : Assignment 6
 * [Description]  :
 * 	- Demonstrates handling "SIGCHLD" signals to prevent zombie process when a child process terminates
 * [Input]	  :
 * 	Nothing
 * [Output]	  :
 * 	Displays messages indicating when the child process starts and exits
 * 	when the "SIGCHLD" signal is received
 * [Calls]	  :
 * 	SigChildHandler(int signo) - Signal handler function for "SIGCHLD"
 * 	pause() - Suspends execution until a signal is received
 * [Special Notes] :
 * 	- Assumes that "SIGCHILD" signal is set up for asynchronous handling of child process termination
 * 	- The program waits indefinitely for signals in the parent process
 */
int main() {

	pid_t pid;

	//Set up the "SIGCHLD" signal handler to handle child process terminate asynchronously
	signal(SIGCHLD, SigChildHandler);

	if((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if(pid == 0) {
		printf("Child.\n");
	}
	else {
		//Parent process waits indefinitely for a signal
		while(1) {
			pause();	//Wait until a signal is received
		}
	}
}
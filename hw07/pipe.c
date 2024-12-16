#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUF 128

/**
 * [Program Name] : Assignment 7
 * [Description]  :
 * 	- A program that implements parent-child process communication using pipes for bidirectional data transfer
 * [Input]	  :
 * 	A string sent from the parent process to the child process(Parent to Child)
 * 	A string sent from the child process to the parent process(Child to Parent)
 * [Output]	  :
 * 	The child process prints the data received from the parent process
 * 	The parent process prints the data received from the child process
 * [Calls]	  :
 * 	pipe()  - Creates a pipe
 * 	fork()  - Creates a child process
 * 	read()  - Reads data from a pipe
 * 	write() - Writes data to a pipe
 * [Special Notes]:
 * 	The parent process sends data to the child process using the first pipe(fd)
 * 	The child process sends data back to the parent process using the second pipe(fd2)
 * 	Each pipe is used independently for bidirectional communication
 * 	The buffer size is defined as MAX_BUF
 */
int main() {
	int n, fd[2], fd2[2];
	pid_t pid;
	char buf[MAX_BUF];

	//Create the first pipe - Parent to Child)
	if (pipe(fd) < 0) {
		perror("pipe");
		exit(1);
	}

	//Create the second pipe - Child to Parent
	if (pipe(fd2) < 0) {
		perror("pipe");
		exit(1);
	}

	//Create a child process using fork
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {	//Child process
		printf("Child: Waiting for Parent to send data\n");

		//Read data from the first pipe(fd[0])
		if ((n = read(fd[0], buf, MAX_BUF)) < 0) {
			perror("read");
			exit(1);
		}		
	
		//Print the data received from teh Parent
		printf("Child: Received data from Parent: ");
		fflush(stdout);		//Immediately flush the output buffer
		write(STDOUT_FILENO, buf, n);	//Write the received data to standard output

		//Send data back to the Parent via the second pipe(fd2[1])
		printf("Child: Sending data to Parent\n");
		strcpy(buf, "Child to Parent\n");
		write(fd2[1], buf, strlen(buf) + 1);	//Write the response message to the pipe
	} else {	//Parent process
		//Send data to the Child via the first pipe(fd[1])
		strcpy(buf, "Parent to Child\n");
		printf("Parent: Sending data to Child\n");
		write(fd[1], buf, strlen(buf) + 1);	//Write the message to the pipe
	
		//Wait for data from the Child
		printf("Parent: Waiting for Child to send data\n");
		if ((n = read(fd2[0], buf, MAX_BUF)) < 0) {
			perror("read");
			exit(1);
		}
	
		//Print the data received from the Child
		printf("Parent: Received data from Child: ");
		fflush(stdout);			//Immediately flush the output buffer
		write(STDOUT_FILENO, buf, n);	//Write the received data to a standard output
	}

	exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD 256	//Maximum length of command input

/**
 * [Function Name]  : int mysystem(char *cmd)
 * [Description]    :
 * 	- This function replicates the behavior of the "system" system call
 * 	- It uses "fork", "execv" and "waitpid" to execute a given command string
 * [Input]	    :
 * 	cmd	- a command string that will be executed by the shell
 * [Output]	    :
 * 	Executes the specified command in a subshell and waits for its completion
 * [Call By]	    :
 * 	Main()
 * [Calls]	    :
 * 	fork()		- Creates a new process
 * 	execv() 	- Executes the command in the child process
 * 	wautoud()	- Waits for the child process to terminate
 * [Given]	    :
 * 	Nothing
 * [Returns]	    :
 * 	On success, it returns the exit status of the child process
 * 	On failure, it returns -1 and outputs an error message
 */
int mysystem(char *cmd) {	//Custom system function implementation

	pid_t pid;	//Variable to store process ID after fork
	int status;	//Variable to store status of child process after execution
	char *argv[] = {"sh", "-c", cmd, NULL};	//Arguments for execv to run the command in a shell

	//Fork a new process
	if((pid = fork()) < 0) {
		perror("fork");
		return -1;
	}
	else if(pid == 0) {	//Child process
		if(execv("/bin/sh", argv) < 0) {	//Execute the command using execv to replace child process with shell command
			perror("execv");
			return -1;
		}
	}

	//Parent process waits for child to finish
	if(waitpid(pid, &status, 0) < 0) {
		perror("waitpid");
		return -1;
	}

	//Return the status of the child process
	return status;
}

/**
 * [Program Name]  : Assignment 5
 * [Description]   :
 * 	- A program to simulate the behavior of the system() function.
 * [Input]	   :
 * 	stdin	- User-provided shell command
 * [Output]	   :
 *     The program outputs the result of the executed command and its status
 * [Calls]	   :
 * 	mysystem()   - Executes the command string as if using the system() function
 * 	fgets()      - Reads user input from standard input
 * 	printf()     - Displays prompts and output to the user
 * 	perror()     - Prints error messages if a function call fails
 * [Special Note]  : Each command is execute by mysystem() which utilizes fork, execv and waitpid to execute the command in a new process
 * 		     Supports continuous command input until "q" is entered to quit the program
 */
main() {

	int status;		//Variable to store status returned by mysystem
	char cmd[MAX_CMD];	//Buffer to store command input from user

	//Command Loop
	while(1) {
		printf("CMD> ");		//Prompt for command input
		fgets(cmd, MAX_CMD, stdin);	//Read command from standard input

		//Check if the user wants to quit by entering "q"
		if(cmd[0] == 'q') {
			break;
		}
		//Execute the command using custom mysystem function
		if((status = mysystem(cmd)) < 0) {
			perror("mysystem");
			exit(1);
		}

		printf("status = %d\n", status);	//Print the exit status of the command
	}
}
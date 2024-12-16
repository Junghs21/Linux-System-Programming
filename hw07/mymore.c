#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int 
main() 
{
	int fd[2];
	pid_t child;

	//Create a pipe
	if(pipe(fd) == -1)  {	//pipe() returns 0 on success, -1 on fail
		perror("pipe");
		exit(1);
	}
				 
	//Create a child proccess using fork()
	child = fork();
	if(child == (pid_t)(-1))  {	//fork() returns -1 on fail, 0 for child, positive value for parent
		perror("fork");
		exit(1);
	}
	else if(child == (pid_t)0) {	//Child process
		close(1);
		close(fd[0]);

		//Redirect the write end of the pipe to standard output
		if(dup(fd[1]) == -1)  {	//dup() duplicate fd[1] to the lowest available file descriptor
			perror("dup");
			exit(1);
		}

		//Replace the child process with the "ls -l" command
		if((execlp("ls", "ls", "-l", NULL)) == -1)  {	//execlp() executes "ls -l"
			perror("execlp");
			exit(1);
		}
	} 
	else {	//Parent process
		close(0);
		close(fd[1]);

		// Redirect the read end of the pipe to standard input using dup2()
		if(dup2(fd[0], 0) == -1)  {	//dup2() redirects fd[0] to file descriptor 0(stdin)
			perror("dup");
			exit(1);
		}
	
		//Replace the parent process with the "more" command
		if((execlp("more", "more", NULL)) == -1)  {	//execlp() executes "more"
			perror("execlp");
			exit(1);
		}
	}

	return 0;
}
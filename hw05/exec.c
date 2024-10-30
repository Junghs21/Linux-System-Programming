#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
	pid_t pid;
	char *argv[] = { "mycp", "exec.c", "exec2.c", NULL };

	//execl
	if((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if(pid == 0) {
		//mycp exec.c exec2.c
		if(execl("/home/cjs/lsp/file-io/mycp", "mycp", "exec.c", "exec1.c", NULL) < 0) {	//Execute the file you received with the list in that path
			perror("execl");
			exit(1);
		}
	}
	if(waitpid(pid, NULL, 0) < 0) {
		perror("waitpid");
		exit(1);
	}

	//execv
	if((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if(pid == 0) {
		//mycp exec.c exec2.c
		if(execv("/home/cjs/lsp/file-io/mycp", argv) <0) {	//Execute the file contained in the vector in that path
			perror("execv");
			exit(1);
		}
	}
	if(waitpid(pid, NULL, 0) < 0) {
		perror("waitpid");
		exit(1);
	}

	//execlp
	if((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if(pid == 0) {
		//execute 'env' from PATH
		//Execute the corresponding filename
		if(execlp("env", "env", NULL) < 0) {	//The first parameter of the exclp function is the program to run, the second is the program renaming, the third parameter NULL indicates that there are no additional factors
			perror("execlp");
			exit(1);
		}
	}
	if(waitpid(pid, NULL, 0) < 0) {
		perror("waitpid");
		exit(1);
	}
}
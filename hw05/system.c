#include <stdio.h>
#include <stdlib.h>

main()
{
	int		status;

	//"system" executes the string of the input command.
	//The child process makes it a fork on its own and executes and waits.

	//Execute "date" command and check for failure
	if ((status = system("date")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	//Esecute a non-existent command "nosuchcommand"
	if ((status = system("nosuchcommand")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	/* $ who; exit 44 */
	if ((status = system("who; exit 44")) < 0)  {	//Execute multiple commands "who" and "exit 44"
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);
}
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "fifo.h"

//Signal handler for SIGINT(Ctrl + C)
void
SigIntHandler(int signo)
{
	//Remove the server FIFO file upon receiving SIGINT
	if (remove(SERV_FIFO) < 0)  {
		perror("remove");
		exit(1);
	}

	exit(0);
}


main()
{
	int			fd, cfd, n;
	MsgType		msg;

	//Register for signal handler for SIGINT
	if (signal(SIGINT, SigIntHandler) == SIG_ERR)  {
		perror("signal");
		exit(1);
	}

	//Create the server FIFO if it doesn't already exist
	if (mkfifo(SERV_FIFO, 0600) < 0)  {
		if (errno != EEXIST)  {	//If the error is not beacause the FIFO already exists
			perror("mkfifo");
			exit(1);
		}
	}

	//Open the server FIFO for read and write access
	if ((fd = open(SERV_FIFO, O_RDWR)) < 0)  {
		perror("open");
		exit(1);
	}

	//Server main loop
	while (1)  {
		//Read a message from the server FIFO
		if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  {
			if (errno == EINTR)  {	//If the read was interrupted by a signal
				continue;	//Restart the read operation
			}
			else  {
				perror("read");
				exit(1);
			}
		}
		printf("Received request: %s.....", msg.data);
	
		//Open the client's FIFO for writing the response
		if ((cfd = open(msg.returnFifo, O_WRONLY)) < 0)  {
			perror("open");
			exit(1);
		}
		//Prepare the response message
		sprintf(msg.data, "This is a reply from %d.", getpid());
		write(cfd, (char *)&msg, sizeof(msg));	//Send the response to the client
		close(cfd);	//Close the client FIFO
		printf("Replied.\n");
	}
}
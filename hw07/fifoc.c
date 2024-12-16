#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "fifo.h"


main()
{
	char		fname[MAX_FIFO_NAME];
	int			fd, sfd, n;
	MsgType		msg;

	//Create a unique FIFO for the client using its process ID
	sprintf(fname, ".fifo%d", getpid());	//Generated FIFO name based on process ID
	if (mkfifo(fname, 0600) < 0)  {	//Create the FIFO with read-write permissions for the owner
		perror("mkfifo");
		exit(1);
	}

	//Open the client FIFO for reading and writing
	if ((fd = open(fname, O_RDWR)) < 0)  {
		perror("open");
		exit(1);
	}
	//Open the server FIFO for reading and writing
	if ((sfd = open(SERV_FIFO, O_RDWR)) < 0)  {
		perror("open");
		exit(1);
	}

	//Prepare a message to send to the server
	strcpy(msg.returnFifo, fname);	//Include the name of the client FIFO in the message
	sprintf(msg.data, "This is a request from %d.", getpid());	//Add request content with process ID
	write(sfd, (char *)&msg, sizeof(msg));	//Send the message to the server via the server FIFO
	printf("Sent a request.....");

	//Wait for a reply from the server
	if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  {
		perror("read");
		exit(1);
	}

	printf("Received reply: %s\n", msg.data);

	close(fd);
	close(sfd);

	//Remove the client FIFO after communication is complete
	if (remove(fname) < 0)  {
		perror("remove");
		exit(1);
	}
}
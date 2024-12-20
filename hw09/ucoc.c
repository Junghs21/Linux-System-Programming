#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "unix.h"


main(int argc, char *argv[])
{
	int					sockfd, n, servAddrLen;
	struct sockaddr_un	servAddr;
	MsgType				msg;

	//Create a UNIX domain socket
	if ((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_STR_PATH);	//Set the socket file path
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	//Connect to the UNIX domain server
	if (connect(sockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("connect");
		exit(1);
	}

	//Prepare and send a request message
	msg.type = MSG_REQUEST;
	sprintf(msg.data, "This is a request from %d.", getpid());
	if (write(sockfd, (char *)&msg, sizeof(msg)) < 0)  {
		perror("write");
		exit(1);
	}
	printf("Sent a request.....");

	//Read the reply from the server
	if ((n = read(sockfd, (char *)&msg, sizeof(msg))) < 0)  {
		perror("read");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);

	//Close the socket
	close(sockfd);
}
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sg.h"


main(int argc, char *argv[])
{
	int					sockfd, n;
	struct sockaddr_in	servAddr;
	MsgType				msg;
	HeaderType			hdr;
	struct iovec		iov[2];	//Scatter, Gather I/O vector

	//Create a TCP socket
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);	//Server address
	servAddr.sin_port = htons(SERV_TCP_PORT);		//Server port

	//Connect to the server
	if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("connect");
		exit(1);
	}

	//Prepare and send a request using scatter, gather I/O
	strcpy(hdr.info, "REQST");	//Set header information
	msg.type = MSG_REQUEST;		//Set message type
	sprintf(msg.data, "This is a request from %d.", getpid());	//Set message content
	iov[0].iov_base = (char *)&hdr;	//First buffer - header
	iov[0].iov_len = sizeof(hdr);
	iov[1].iov_base = (char *)&msg;	//Second buffer - message
	iov[1].iov_len = sizeof(msg);
	if (writev(sockfd, iov, 2) < 0)  {	//Send both buffers
		perror("write");
		exit(1);
	}
	printf("Sent a request.....");									

	//Receive a reply using scatter, gather I/O
	if ((n = readv(sockfd, iov, 2)) < 0)  {	//Read into both buffers
		perror("read");
		exit(1);
	}
	printf("Received reply: %s(%s)\n", msg.data, hdr.info);

	//Close the socket
	close(sockfd);
}
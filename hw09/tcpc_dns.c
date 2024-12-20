#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "tcp.h"


main(int argc, char *argv[])
{
	int					sockfd, n;
	struct sockaddr_in	servAddr;
	MsgType				msg;
	struct hostent		*hp;	//For DNS resolution

	//Check for correct usage
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s IPaddress\n", argv[0]);
		exit(1);
	}

	//Create a TCP socket
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port = htons(SERV_TCP_PORT);	//Use predefined server port

	//Resolve server address(IP or hostname)
	if (isdigit(argv[1][0]))  {
		//If the input is an IP address, use inet_addr
		servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else  {
		//If the input is a hostname, resolve it using gethostbyname
		if ((hp = gethostbyname(argv[1])) == NULL)  {
			fprintf(stderr, "Unknown host: %s\n", argv[1]);
			exit(1);
		}
		memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
	}

	//Connect to the server
	if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"

#define	MSG_MODE	(0600)


main()
{
	int		msqid, n;
	MsgType	msg;


	//Connect to the message queue using the specified key
	if ((msqid = msgget(MSG_KEY, MSG_MODE)) < 0)  {
		perror("msgget");
		exit(1);
	}

	//Prepare and send a request message
	msg.type = MSG_REQUEST;		//Set the message type to request
	sprintf(msg.data, "This is a request from %d.", getpid());	//Format the request message with process ID
	if (msgsnd(msqid, (char *)&msg, sizeof(msg), 0) < 0)  {
		perror("msgsnd");
		exit(1);
	}
	printf("Sent a request.....");

	//Wait for a reply from the server
	if ((n = msgrcv(msqid, (char *)&msg, sizeof(msg), MSG_REPLY, 0)) < 0)  {
		perror("msgrcv");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);
}
/*
 * clientSocket.c
 *
 *  Created on: Jan 19, 2015
 *      Author: adminuser
 */
#include "webclient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


int socketInit ()
{
	int newSocket;

	printf("INFO:[%010u] Init socket\n", pthread_self());

	/* make a socket */
	newSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(newSocket == SOCKET_ERROR)
	{
		printf("\n*** Could not make a socket\n");
		return 0;
	}

	return newSocket;
}

int serverConnect(char* srvName, int srvPort, int sock)
{
	long srvAddress;
	struct hostent* hostInfo;   /* holds info about a machine */
	struct sockaddr_in Address; /* Internet socket address stuct */
	int addressSize=sizeof(struct sockaddr_in);

	printf("INFO:[%010u] Server Name: %s\n", pthread_self(), srvName);
	/* get IP address from name */
	hostInfo=gethostbyname(srvName);
	/* copy address into long */
	memcpy(&srvAddress,hostInfo->h_addr,hostInfo->h_length);

	/* fill address struct */
	Address.sin_addr.s_addr=srvAddress;
	Address.sin_port=htons(srvPort);
	Address.sin_family=AF_INET;

	printf("INFO:[%010u] Connecting to [%s] on port [%d]\n", pthread_self(), srvName,srvPort);

	if(connect(sock,(struct sockaddr*)&Address,sizeof(Address))
			   == SOCKET_ERROR)
	{
		printf("\n*** Could not connect to server\n");
		exit(0);
	}

	getsockname(sock, (struct sockaddr *) &Address,(socklen_t *)&addressSize);
	printf("INFO:[%010u] Opened socket as fd (%d) on port (%d) for stream i/o\n", pthread_self(), sock, ntohs(Address.sin_port) );

	printf("Server\n\
		  sin_family        = %d\n\
		  sin_addr.s_addr   = %d\n\
		  sin_port          = %d\n"
		  , Address.sin_family
		  , Address.sin_addr.s_addr
		  , ntohs(Address.sin_port)
		);
	return 0;
}

/*
 * webclient.c
 *
 *  Created on: Jan 15, 2015
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
#include <fcntl.h> /* O_WRONLY, O_CREAT */


void error (const char *msg)
{
perror (msg);
}

int recv_file(int sock, char* file_name, unsigned long fSize)
{
	//char sendBuff [SEND_BUFFER_SIZE]; /* message to be sent to server*/
	int f; /* file handle for receiving file*/
	ssize_t sentBytes, recvBytes, rcvd_file_size, writtenBytes;
	int recv_count; /* count of recv() calls*/
	char recvBuff[RECV_BUFFER_SIZE]; /* buffer to hold received data */

	/* attempt to create file to save received data. 0644 = rw-r--r-- */
	if ( (f = open(file_name, O_WRONLY|O_CREAT, 0644)) < 0 )
	{
		perror("error creating file");
		return -1;
	}
	recv_count = 0; /* number of recv() calls required to receive the file */
	rcvd_file_size = 0; /* size of received file */
	/* continue receiving until ? (data or close) */

	printf("Filesize is [%lu]\n",fSize);
	while ((recvBytes = recv(sock, recvBuff, RECV_BUFFER_SIZE, 0)) > 0)
	{
		recv_count++;
		rcvd_file_size += recvBytes;
		writtenBytes = write(f, recvBuff, recvBytes);
		/*if (write(f, recvBuff, recvBytes) < 0 )
		{
			perror("error writing to file");
			return -1;
		}*/
		fSize = fSize - recvBytes;
		printf("Received bytes:[%d] write file bytes:[%d] bytes remaining:[%d]\n",recvBytes, writtenBytes, fSize);
	}
	close(f); /* close file*/
	printf("Client Received: %d bytes in %d recv(s)\n", rcvd_file_size,	recv_count);
	return rcvd_file_size;
}

int main(int argc, char* argv[])
{
    int clientSocket;               	  	/* handle to socket */
    char recvBuffer[RECV_BUFFER_SIZE];
    char sendBuffer[SEND_BUFFER_SIZE];
    char tmpSendBuffer[SEND_BUFFER_SIZE];
    char tmpRecvBuffer[RECV_BUFFER_SIZE];
    char serverName[HOST_NAME_SIZE];
    int serverPort;
    unsigned long fileSz;
    size_t sentBytes, readBytes;
    char *fileNameToken, *getFileCmdToken, *getToken;
    char *getFileToken, *getStatusToken, *fileSzToken;

    strcpy(serverName,argv[1]);
	serverPort=atoi(argv[2]);


    /* Initialize socket */
    clientSocket = socketInit();
    serverConnect(serverName, serverPort, clientSocket);

    bzero((char *) &recvBuffer, sizeof(recvBuffer)+1);
    bzero((char *) &sendBuffer, sizeof(sendBuffer)+1);
	printf("\nEnter GetFile cmd: \n");
	fgets(sendBuffer,SEND_BUFFER_SIZE,stdin);

	/*Parse GetFile request */
	printf("Parsing Request\n");
	printf("===============\n");
	strcpy(tmpSendBuffer,sendBuffer);
	getFileCmdToken = strtok(tmpSendBuffer, " ");
	getToken = strtok(NULL, " ");
	fileNameToken = strtok(NULL, " ");
	printf( "[%s][%s][%s]\n", getFileCmdToken, getToken, fileNameToken);

	/*Send the GetFile Req to Server */
	printf("Sending Request\n");
	printf("===============\n");
	sentBytes = send(clientSocket,sendBuffer,SEND_BUFFER_SIZE,0);
	printf("Sending cmd:[%s] sent bytes:[%d]\n",sendBuffer,sentBytes);

	/*Read the GetFile Res from Server */
	printf("Reading Response\n");
	printf("================\n");
	readBytes = recv(clientSocket,recvBuffer,RECV_BUFFER_SIZE,0);
	printf("Received response:[%s] recv bytes:[%d]\n",recvBuffer,readBytes);
	/* Exit if file is not found on server */
	if (strstr(recvBuffer, GETFILE_FILE_NOT_FOUND) != NULL)
	{
		printf("\n[%s]\n",recvBuffer);
		return (0);
	}

	/*Parse the GetFile Res messge */
	printf("Parsing Response\n");
	printf("================\n");
	strcpy(tmpRecvBuffer,recvBuffer);
	getFileToken = strtok(tmpRecvBuffer, " ");
	getStatusToken = strtok(NULL, " ");
	fileSzToken = strtok(NULL, " ");
	printf( "[%s][%s][%s]\n", getFileToken, getStatusToken, fileSzToken );
	trimwhitespace(fileNameToken);

	/* Convert fileSzToken to int */
	fileSz = strtol (fileSzToken, NULL, 10);

	printf("Receiving File: [%s]\n", fileNameToken);
	printf("======================================\n");
	recv_file(clientSocket, fileNameToken, fileSz);

	printf("\nClosing socket\n");
			/* close socket */
	if(close(clientSocket) == SOCKET_ERROR)
	{
		printf("\nCould not close socket\n");

	}
return 0;
}

/*
 * clientFunc.c
 *
 *  Created on: Jan 28, 2015
 *      Author: adminuser
 */

#include "webclient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h> /* open, O_RDONLY */


void *serverReq(int socket)
{
	char sendBuffer[SEND_BUFFER_SIZE];
	char recvBuffer[RECV_BUFFER_SIZE];
	char tmpRecvBuffer[RECV_BUFFER_SIZE];
	char tmpSendBuffer[RECV_BUFFER_SIZE];
	char *fileNameToken, *getFileCmdToken, *getToken;
	char *getFileToken, *getStatusToken, *fileSzToken;
	char *tmpFilePath;
	char *sendFilePath;
	unsigned long fileSz;
	ssize_t recvBytes;
	ssize_t sentBytes;


	bzero((char *) &sendBuffer, sizeof(sendBuffer)+1);
	bzero((char *) &recvBuffer, sizeof(recvBuffer)+1);
	/*Parse GetFile request */
	printf("\nEnter GetFile cmd: \n");
	fgets(sendBuffer,SEND_BUFFER_SIZE,stdin);

	printf("INFO:Parsing Request\n");
	strcpy(tmpSendBuffer,sendBuffer);
	getFileCmdToken = strtok(tmpSendBuffer, " ");
	getToken = strtok(NULL, " ");
	fileNameToken = strtok(NULL, " ");
	printf( "DEBUG:[%s][%s][%s]\n", getFileCmdToken, getToken, fileNameToken);

	/*Send the GetFile Req to Server */
	printf("INFO:Sending Request\n");
	sentBytes = send(socket,sendBuffer,SEND_BUFFER_SIZE,0);
	printf("INFO:Sending cmd:[%s] sent bytes:[%d]\n",sendBuffer,sentBytes);

	/*Read the GetFile Res from Server */
	recvBytes = recv(socket,recvBuffer,RECV_BUFFER_SIZE,0);
	printf("INFO:Recd response:[%s] recv bytes:[%d]\n",recvBuffer,recvBytes);
	/* Exit if file is not found on server */
	if (strstr(recvBuffer, GETFILE_FILE_NOT_FOUND) != NULL)
	{
		printf("DEBUG:[%s]\n",recvBuffer);
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
	recvFile(socket, fileNameToken, fileSz);
	return 0;

}

int recvFile(int sock, char* file_name, unsigned long fSize)
{
	//char sendBuff [SEND_BUFFER_SIZE]; /* message to be sent to server*/
	int f; /* file handle for receiving file*/
	ssize_t sentBytes;
	ssize_t recvBytes;
	ssize_t recvFileSize;
	ssize_t writtenBytes;
	int recvCount; /* count of recv() calls*/
	char recvBuff[RECV_BUFFER_SIZE]; /* buffer to hold received data */

	/* attempt to create file to save received data. 0644 = rw-r--r-- */
	if ( (f = open(file_name, O_WRONLY|O_CREAT, 0644)) < 0 )
	{
		perror("error creating file");
		return -1;
	}
	recvCount = 0; /* number of recv() calls required to receive the file */
	recvFileSize = 0; /* size of received file */
	printf("DEBUG:Filesize is [%lu]\n",fSize);
	while ((recvBytes = recv(sock, recvBuff, RECV_BUFFER_SIZE, 0)) > 0)
	{
		recvCount++;
		recvFileSize += recvBytes;
		writtenBytes = write(f, recvBuff, recvBytes);
		/*if (write(f, recvBuff, recvBytes) < 0 )
		{
			perror("error writing to file");
			return -1;
		}*/
		fSize = fSize - recvBytes;
		printf("INFO:Received bytes:[%d] write file bytes:[%d] bytes remaining:[%d]\n",recvBytes, writtenBytes, fSize);
	}
	close(f); /* close file*/
	printf("####: Client Received: %d bytes in %d recv(s)\n", recvFileSize,	recvCount);
	return recvFileSize;
}


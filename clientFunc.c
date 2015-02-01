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

void *serverReq(int reqThread)
{
	char sendBuffer[SEND_BUFFER_SIZE];
	char recvBuffer[RECV_BUFFER_SIZE];
	char tmpRecvBuffer[RECV_BUFFER_SIZE];
	char tmpSendBuffer[SEND_BUFFER_SIZE];
	char *fileNameToken, *getFileCmdToken, *getToken;
	char *getFileToken, *getStatusToken, *fileSzToken;
	char *tmpFilePath;
	char *sendFilePath;
	char *targetFileName;
	unsigned long fileSz;
	ssize_t recvBytes;
	ssize_t sentBytes;
	int clientSocket;
	int reqDone = 0;
	FILE * fp;
	struct timeval start, end;


	printf("INFO:[%010u] Entered serverReq\n", pthread_self());
	printf("INFO:[%010u] Requests needed from thread:[%d]\n", pthread_self(), (int) reqThread);
	targetFileName = (char *) malloc(sizeof(char) * 1024);

	while (reqDone < reqThread)
	{
		pthread_mutex_lock (&mtex);

		while (emptyQ() == 1)
		{  /* block if buffer empty */
			printf("INFO:[%010u] waiting ...\n",pthread_self());
			pthread_cond_wait (&mtex_cond, &mtex);
		}
		clientSocket = socketInit();
		serverConnect(serverAddr, serverPort, clientSocket);
		/* Get the next request from queue */
		deQ(targetFileName);
		queueCount--;
		threadsActive++;
		threadsWaiting--;
		printf("INFO:[%010u] deQ filename[%s]\n", pthread_self(), targetFileName);
		pthread_mutex_unlock (&mtex);


		bzero((char *) &sendBuffer, sizeof(sendBuffer)+1);
		bzero((char *) &tmpSendBuffer, sizeof(tmpSendBuffer)+1);
		bzero((char *) &recvBuffer, sizeof(recvBuffer)+1);
		bzero((char *) &tmpRecvBuffer, sizeof(recvBuffer)+1);
		/*Parse GetFile request */
		//printf("\nEnter GetFile cmd: \n");
		//fgets(sendBuffer,SEND_BUFFER_SIZE,stdin);
		//sprintf(sendBuffer, "%s", "GetFile GET 10mb-sample-file-0.mpg");
		sprintf(sendBuffer, "%s%s", GETFILE_CMD, targetFileName);
		printf("INFO:[%010u] Parsing REQ[%s]\n", pthread_self(), sendBuffer);
		strcpy(tmpSendBuffer,sendBuffer);
		getFileCmdToken = strtok(tmpSendBuffer, " ");
		getToken = strtok(NULL, " ");
		fileNameToken = strtok(NULL, " ");
		printf( "INFO:[%010u] Parsed REQ[%s][%s][%s]\n", pthread_self(), getFileCmdToken, getToken, fileNameToken);

		//
		/*Send the GetFile Req to Server */
		sentBytes = send(clientSocket,sendBuffer,SEND_BUFFER_SIZE,0);
		printf("INFO:[%010u] Send REQ[%s] sent bytes:[%d] fd:[%d]\n",pthread_self(), sendBuffer, sentBytes, clientSocket);

		/*Read the GetFile Res from Server */
		recvBytes = recv(clientSocket,recvBuffer,RECV_BUFFER_SIZE,0);
		printf("INFO:[%010u] Recd RES[%s] recv bytes:[%d]\n",pthread_self(), recvBuffer, recvBytes);

		/* Exit if file is not found on server */
		if (strstr(recvBuffer, GETFILE_FILE_NOT_FOUND) != NULL)
		{
			printf("ERROR:[%010u][%s]\n",pthread_self(),recvBuffer);
			break;
		}
		/*Parse the GetFile Res messge */
		printf("INFO:[%010u] Parsing RES[%s]\n",pthread_self(),recvBuffer);
		strcpy(tmpRecvBuffer,recvBuffer);
		getFileToken = strtok(tmpRecvBuffer, " ");
		getStatusToken = strtok(NULL, " ");
		fileSzToken = strtok(NULL, " ");
		printf("INFO:[%010u] Parsed RES[%s][%s][%s]\n", pthread_self(), getFileToken, getStatusToken, fileSzToken );
		trimwhitespace(fileNameToken);

		/* Convert fileSzToken to int */
		fileSz = strtol (fileSzToken, NULL, 10);

		//printf("DEBUG:[%010u] Rec File: [%s]\n", pthread_self(), fileNameToken);


		if ( ((fp = fopen(fileNameToken, "r")) != NULL) || (strcmp(downloadPath, "") == 0) )
		{
			if (fp != NULL) fclose(fp);
			if ( (recvFileNoWrite(clientSocket, fileNameToken, fileSz)) > 0)
			{
				pthread_mutex_lock (&mtex);
				gtotalReqDone++;
				printf("====>[%010u] Total Requests Done:[%d]\n", pthread_self(), gtotalReqDone);
				pthread_mutex_unlock (&mtex);
			}

		}
		else if (fp == NULL)
		{
			if ( (recvFile(clientSocket, fileNameToken, fileSz)) > 0)
			{
				pthread_mutex_lock (&mtex);
				gtotalReqDone++;
				printf("====> Total Requests Done:[%d]\n", gtotalReqDone);
				pthread_mutex_unlock (&mtex);
			}
		}

		if(close(clientSocket) == SOCKET_ERROR)
		{
			error("ERROR: Could not close CLIENT socket\n");
		}
		reqDone++;
	} /* While */

	free(targetFileName);
	pthread_exit(0);
	return NULL;
}

int recvFile(int sock, char* file_name, unsigned long fSize)
{
	int f; /* file handle for receiving file*/
	FILE * fp;
	ssize_t recvBytes;
	ssize_t recvFileSize;
	ssize_t writtenBytes;
	int recvCount; /* count of recv() calls*/
	char recvBuff[RECV_BUFFER_SIZE]; /* buffer to hold received data */
	unsigned long reqElapsedTime = 0;
	struct timeval start, end;


	/* attempt to create file to save received data. 0644 = rw-r--r-- */

	/*if ( (f = open(file_name, O_WRONLY|O_CREAT, 0644)) < 0 )
	{
		error("ERROR: error creating file");
		return -1;
	}*/
	fp = fopen(file_name, "w");
	if (fp == NULL)
	{
		error("ERROR: error creating file");
		return -1;
	}

	gettimeofday(&start, NULL);
	recvCount = 0; /* number of recv() calls required to receive the file */
	recvFileSize = 0; /* size of received file */
	printf("INFO:[%010u] Filesize is [%lu]\n",pthread_self(), fSize);
	while ((recvBytes = recv(sock, recvBuff, RECV_BUFFER_SIZE, 0)) > 0)
	{
		recvCount++;
		recvFileSize += recvBytes;
		//writtenBytes = write(fp, recvBuff, recvBytes);
		writtenBytes = fwrite(recvBuff, sizeof(char), recvBytes, fp);
		/* if ( (writtenBytes = write(fp, recvBuff, recvBytes)) < 0 )
		{
			error("ERROR: error writing to file");
			return -1;
		} */
		fSize = fSize - recvBytes;
		printf("DEBUG:[%010u] Recd bytes:[%d] write file bytes:[%d] bytes remaining:[%d]\n",pthread_self(),recvBytes, writtenBytes, fSize);
	}
	close(fp); /* close file*/
	printf("====>[%010u] Recd file:[%s][%d bytes][%d recvs]\n", pthread_self(), file_name, recvFileSize,	recvCount);

	/* Metrics collection */
	gettimeofday(&end, NULL);
	reqElapsedTime = (end.tv_sec * 1000000 + end.tv_usec)
							  - (start.tv_sec * 1000000 + start.tv_usec);
	pthread_mutex_lock (&mtex);
	totalRecvBytes += recvFileSize;
	enQMetrics(reqElapsedTime);
	pthread_mutex_unlock (&mtex);

	return recvFileSize;
}

/* =====================================================
 * recvFileNoWrite
 */
int recvFileNoWrite(int sock, char* file_name, unsigned long fSize)
{
	ssize_t recvBytes;
	ssize_t recvFileSize;
	int recvCount; /* count of recv() calls*/
	char recvBuff[RECV_BUFFER_SIZE]; /* buffer to hold received data */
	unsigned long reqElapsedTime = 0;
	struct timeval start, end;

	gettimeofday(&start, NULL);
	recvCount = 0; /* number of recv() calls required to receive the file */
	recvFileSize = 0; /* size of received file */
	printf("INFO:[%010u] Streaming file only - no write\n",pthread_self());
	printf("INFO:[%010u] Filesize is [%lu]\n",pthread_self(), fSize);
	while ((recvBytes = recv(sock, recvBuff, RECV_BUFFER_SIZE, 0)) > 0)
	{
		recvCount++;
		recvFileSize += recvBytes;
		fSize = fSize - recvBytes;
		//printf("INFO:[%010u] Recd bytes:[%d] write file bytes:[%d] bytes remaining:[%d]\n",pthread_self(),recvBytes, writtenBytes, fSize);
	}
	printf("====>[%010u] Recd file:[%s][%d bytes][%d recvs]\n", pthread_self(), file_name, recvFileSize,	recvCount);
	gettimeofday(&end, NULL);
	reqElapsedTime = (end.tv_sec * 1000000 + end.tv_usec)
							  - (start.tv_sec * 1000000 + start.tv_usec);
	pthread_mutex_lock (&mtex);
	totalRecvBytes += recvFileSize;
	enQMetrics(reqElapsedTime);
	pthread_mutex_unlock (&mtex);
	return recvFileSize;
}

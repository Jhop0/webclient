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

void usage(void)
{
	printf("Usage:\n");
	printf("\twebclient [options]\n");
	printf("options:\n");
	printf("\t-p port (Default: 8888)\n");
	printf("\t-t number of worker threads (Default: 1, Range: 1-100\n");
	printf("\t-w path to workload file (Default: workload.txt\n");
	printf("\t-d path to download file directory (Default: null)\n");
	printf("\t-r number of total requests (Default: 10, Range: 1-1000)\n");
	printf("\t-m path to metrics file (Default: metrics.txt)\n");
	printf("\t-h show help message\n");
	exit (8);
}

int main(int argc, char* argv[])
{
    int clientSocket;               	  	/* handle to socket */
    char recvBuffer[RECV_BUFFER_SIZE];
    char sendBuffer[SEND_BUFFER_SIZE];
    char tmpSendBuffer[SEND_BUFFER_SIZE];
    char tmpRecvBuffer[RECV_BUFFER_SIZE];
    char serverName[HOST_NAME_SIZE];
    //int serverPort;
    unsigned long fileSz;
    size_t sentBytes, readBytes;
    char *fileNameToken, *getFileCmdToken, *getToken;
    char *getFileToken, *getStatusToken, *fileSzToken;
    int opt=0;

	/* Init the globals */
    serverAddr = DEF_SERVER_ADDR;
	serverPort = DEF_SERVER_PORT;
	workloadPath = DEF_WLOAD_PATH;
	downloadPath = DEF_DLOAD_PATH;
	metricsPath = DEF_METRICS_PATH;
	int workerThreads = DEF_WORKER_THREADS;
	int totalReq = DEF_TOTAL_REQ;

    while ((opt = getopt(argc, argv, "s:p:t:w:d:r:m:h")) != -1)
	{
		switch(opt)
		{
			case 's':
				serverAddr = optarg;
				printf("\nOption value=%d", serverAddr);
				break;
			case 'p':
				serverPort = atoi(optarg);
				printf("\nOption value=%d", serverPort);
				break;
			case 't':
				workerThreads = atoi(optarg);;
				printf("\nOption value=%d", workerThreads);
				break;
			case 'w':
				workloadPath = optarg;
				trimwhitespace(workloadPath);
				printf("\nOption value=%s\n", workloadPath);
				break;
			case 'd':
				downloadPath = optarg;
				trimwhitespace(downloadPath);
				printf("\nOption value=%s\n", downloadPath);
				break;
			case 'r':
				totalReq = atoi(optarg);
				printf("\nOption value=%d\n", totalReq);
				break;
			case 'm':
				metricsPath = optarg;
				printf("\nOption value=%s\n", metricsPath);
				break;
			case 'h':
				usage();
				break;
			case '?':
				printf("\nMissing argument(s)");
				usage();
				break;
		 }
	 }


    //strcpy(serverName,argv[1]);
	//serverPort=atoi(argv[2]);

    /* Initialize socket */
    clientSocket = socketInit();
    serverConnect(serverAddr, serverPort, clientSocket);

    serverReq(clientSocket);

	printf("\nClosing socket\n");
			/* close socket */
	if(close(clientSocket) == SOCKET_ERROR)
	{
		printf("\nCould not close socket\n");

	}
return 0;
}

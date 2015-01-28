/*
 * webclient.h
 *
 *  Created on: Jan 24, 2015
 *      Author: adminuser
 */

#ifndef WEBCLIENT_H_
#define WEBCLIENT_H_

#define SOCKET_ERROR        -1
#define QUEUE_SIZE          5
#define HOST_NAME_SIZE      255
#define GETFILE_PATH_MAX			255

#define RECV_BUFFER_SIZE		4096
#define SEND_BUFFER_SIZE		4096
#define GETFILE_TEST			"1kb-sample-file-0.png"
#define GETFILE_STATUS_ERR		"GetFile FILE_NOT_FOUND 0 0"
#define GETFILE_STATUS_OK		"GetFile OK "
#define GETFILE_FILE_NOT_FOUND	"FILE_NOT_FOUND"

/* API Defaults */
#define DEF_SERVER_ADDR				"0.0.0.0"		/* -s address Server address */
#define DEF_SERVER_PORT				8888			/* -p port Server listening port */
#define DEF_WORKER_THREADS			1 				/* -t number of worker threads */
#define DEF_WLOAD_PATH			"./workload.txt" 	/* -w path to workload.txt */
#define DEF_DLOAD_PATH			"./"				/* -d path to download file dir */
#define DEF_TOTAL_REQ			10					/* -r number of total requests */
#define DEF_METRICS_PATH		"./metrics.txt"		/* -m path to metrics.txt */


/* Global vars */
int serverAddr;
int serverPort;
char *workloadPath;
char *downloadPath;
char *metricsPath;
int queueCount;
int threadsActive;
int threadsWaiting;
//int sentFileCount;

/* webclient */
void error (const char*);
void usage(void);

/* clientSocket */
int socketInit ();
int serverConnect(char*, int, int);

/* clientFunc */
void * serverReq(int);
int recvFile(int ,char*, unsigned long);

/* stringOps */
void removeSubstring(char*, const char*);
char* trimwhitespace(char*);

/* queueOps */
int frontelement();
void enQ(int);
int deQ();
int emptyQ();
void displayQ();
void createQ();
int sizeQ();

#endif /* WEBCLIENT_H_ */

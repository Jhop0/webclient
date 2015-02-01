/*
 * webclient.h
 *
 *  Created on: Jan 24, 2015
 *      Author: adminuser
 */

#ifndef WEBCLIENT_H_
#define WEBCLIENT_H_

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> /* O_WRONLY, O_CREAT */
#include <time.h>
#include <sys/time.h>

#define SOCKET_ERROR        -1
#define QUEUE_SIZE          20
#define HOST_NAME_SIZE      255
#define GETFILE_PATH_MAX	255

#define RECV_BUFFER_SIZE		4096
#define SEND_BUFFER_SIZE		4096
#define GETFILE_CMD				"GetFile GET "
#define GETFILE_STATUS_ERR		"GetFile FILE_NOT_FOUND 0 0"
#define GETFILE_STATUS_OK		"GetFile OK "
#define GETFILE_FILE_NOT_FOUND	"FILE_NOT_FOUND"
#define WORKER_THREADS_MAX		100
#define REQUEST_MAX				1000
#define FILENAME_MAX_SIZE		200

/* API Defaults */
#define DEF_SERVER_ADDR				"0.0.0.0"		/* -s address Server address */
#define DEF_SERVER_PORT				8888			/* -p port Server listening port */
#define DEF_WORKER_THREADS			1 				/* -t number of worker threads */
#define DEF_WLOAD_PATH			"./workload.txt" 	/* -w path to workload.txt */
#define DEF_DLOAD_PATH			""				/* -d path to download file dir */
#define DEF_TOTAL_REQ			10					/* -r number of total requests */
#define DEF_METRICS_PATH		"./metrics.txt"		/* -m path to metrics.txt */


/* Global vars */
char *serverAddr;
int serverPort;
char *workloadPath;
char *downloadPath;
char *metricsPath;
int queueCount;
int threadsActive;
int threadsWaiting;
int gtotalReqDone;
FILE * g_fpMetrics;

long unsigned totalElapsedTime;
long unsigned perThreadElapsedTime;
ssize_t totalRecvBytes;
//int sentFileCount;

/* Create mutex for queue */
pthread_mutex_t mtex;  	/* mutex lock for buffer */
pthread_cond_t mtex_cond;

/* webclient */
void error (const char*);
void usage(void);

/* clientSocket */
int socketInit ();
int serverConnect(char*, int, int);

/* clientFunc */
void * serverReq(int);
int recvFile(int, char*, unsigned long);
int recvFileNoWrite(int, char*, unsigned long);

/* stringOps */
void removeSubstring(char*, const char*);
char* trimwhitespace(char*);

/* queueOps */
char * frontelement();
void enQ(char *);
int deQ(char *);
int emptyQ();
void displayQ();
void createQ();
int sizeQ();
void createQMetrics();
void enQMetrics(unsigned long);
unsigned long deQMetrics();
int emptyQMetrics();

#endif /* WEBCLIENT_H_ */

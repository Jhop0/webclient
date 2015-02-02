/*
 * webclient.c
 *
 *  Created on: Jan 15, 2015
 *      Author: adminuser
 */

#include "webclient.h"



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
    char fileName[FILENAME_MAX_SIZE + 1];
    pthread_t tid[WORKER_THREADS_MAX];
    int i, j=0, opt=0;
    int reqPerThread = 0;
    int workerThreads = DEF_WORKER_THREADS;
	int totalReq = DEF_TOTAL_REQ;
	FILE * fpWorkload;
	struct timeval start,end;
	struct stat dirInfo;

	/* Init the globals */
    serverAddr = DEF_SERVER_ADDR;
	serverPort = DEF_SERVER_PORT;
	workloadPath = DEF_WLOAD_PATH;
	downloadPath = DEF_DLOAD_PATH;
	metricsPath = DEF_METRICS_PATH;

	gtotalReqDone = 0;

	/* Init the mutex */
	pthread_mutex_t mtex = PTHREAD_MUTEX_INITIALIZER;  	/* mutex lock for buffer */
	//pthread_cond_t mtex_cond = PTHREAD_COND_INITIALIZER; /* producer waits on this cond var */
	pthread_cond_init(&mtex_cond, NULL);

    while ((opt = getopt(argc, argv, "s:p:t:w:d:r:m:h")) != -1)
	{
		switch(opt)
		{
			case 's':
				serverAddr = optarg;
				printf("Server address:[%s]\n", serverAddr);
				break;
			case 'p':
				serverPort = atoi(optarg);
				printf("Server port:[%d]\n", serverPort);
				break;
			case 't':
				workerThreads = atoi(optarg);;
				printf("Worker threads:[%d]\n", workerThreads);
				if ( (workerThreads > WORKER_THREADS_MAX) || (totalReq < 1) ) usage();
				break;
			case 'w':
				workloadPath = optarg;
				trimwhitespace(workloadPath);
				printf("Workload filename:[%s]\n", workloadPath);
				break;
			case 'd':
				downloadPath = optarg;
				trimwhitespace(downloadPath);
				printf("Download path:[%s]\n", downloadPath);
				break;
			case 'r':
				totalReq = atoi(optarg);
				printf("Total requests:[%d]\n", totalReq);
				if ( (totalReq > REQUEST_MAX) || (totalReq < 1)) usage();
				break;
			case 'm':
				metricsPath = optarg;
				printf("Metrics filename:[%s]\n", metricsPath);
				break;
			case 'h':
				usage();
				break;
			case '?':
				printf("Missing argument(s)\n");
				usage();
				break;
		 }
	 }

    printf("=========================================\n");
    printf("Webclient Init Params\n");
    printf("=========================================\n");
    printf("Server address:\t\t[%s]\n", serverAddr);
    printf("Server port:\t\t[%d]\n", serverPort);
    printf("Worker threads:\t\t[%d]\n", workerThreads);
    printf("Workload filename:\t[%s]\n", workloadPath);
    printf("Download path:\t\t[%s]\n", downloadPath);
    printf("Total requests:\t\t[%d]\n", totalReq);
    printf("Metrics filename:\t[%s]\n", metricsPath);
    printf("==========================================\n");

    fpWorkload = fopen(workloadPath, "r");
	//printf("DEBUG: Workload:[%s], fd:[%d]\n", workloadPath, fpWorkload);
	if ( fpWorkload == NULL )
	{
		perror("ERROR: Error reading workload file\n");
		return -1;
	}

	g_fpMetrics = fopen(metricsPath, "w");
	//printf("DEBUG: Workload:[%s], fd:[%d]\n", metricsPath, fpMetrics);
	if ( g_fpMetrics == NULL )
	{
		perror("ERROR: Error opening metrics file\n");
		return -1;
	}



	if( stat( downloadPath, &dirInfo ) != 0 )
	{
		perror("ERROR: Download path not valid\n");
		return -1;
	}

    /* Create request Q */
	createQ();
	createQMetrics();

	/* Create worker thread pool */
	printf("--> Creating [%d] worker threads .... \n", workerThreads);
	int tmpWorkerThreads=workerThreads;
	if (totalReq <= workerThreads)
	{
		tmpWorkerThreads = totalReq;
		reqPerThread = 1;
	}
	else
	{
		reqPerThread = totalReq / tmpWorkerThreads;
	}

	for (i = 1; i < (tmpWorkerThreads+1); i++)
	{
		if (totalReq == tmpWorkerThreads)
		{
			pthread_create(&tid[i], NULL, serverReq, reqPerThread);
		}
		else
		{
			if (i <= (totalReq % tmpWorkerThreads))
			{
				pthread_create(&tid[i], NULL, serverReq, reqPerThread+1);
			}
			else
			{
				pthread_create(&tid[i], NULL, serverReq, reqPerThread);
			}

		} /* If */

	} /* For */

	/* Metrics */
	threadsWaiting = tmpWorkerThreads;
	gettimeofday(&start, NULL);

	for(j=0; j < totalReq; j++)
	{
		if ( (fgets(fileName, FILENAME_MAX_SIZE, fpWorkload) == NULL) ) rewind(fpWorkload);
		pthread_mutex_lock (&mtex);
		enQ(fileName);
		pthread_cond_broadcast (&mtex_cond);
		//displayQ();
		pthread_mutex_unlock (&mtex);

	}
	fclose(fpWorkload);

	for (i = 1; i < tmpWorkerThreads+1; i++)
	{
		pthread_join(tid[i], NULL);
	}
	gettimeofday(&end, NULL);

	/* DeQ the metrics data */
	while (emptyQMetrics() != 1)
	{
		perThreadElapsedTime += deQMetrics();
	}
	fclose(g_fpMetrics);

	totalElapsedTime = (end.tv_sec * 1000000 + end.tv_usec)
						  - (start.tv_sec * 1000000 + start.tv_usec);
	printf("COMPLETION: Total recv bytes:[%lu]\n", totalRecvBytes);
	printf("COMPLETION: Total client runtime:[%lu]usec/[%lu]msec\n", totalElapsedTime, totalElapsedTime / 1000);
	printf("COMPLETION: Per request cumulative runtime:[%lu]usec/[%lu]msec\n", perThreadElapsedTime, perThreadElapsedTime / 1000);
	printf("COMPLETION: Average response time:[%lu]usec/[%lu]msec per thread\n", (perThreadElapsedTime / tmpWorkerThreads),(perThreadElapsedTime / 1000 / tmpWorkerThreads));
	printf("COMPLETION: Average throughput:[%lu]Bytes/usec [%lu]Bytes/sec \n", (totalRecvBytes/(perThreadElapsedTime / tmpWorkerThreads)),(totalRecvBytes*1000000/(perThreadElapsedTime / tmpWorkerThreads)));



return 0;
}

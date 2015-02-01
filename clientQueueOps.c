/*
 * clientQueueOps.c
 *
 *  Created on: Jan 28, 2015
 *      Author: adminuser
 */

/*
 * Queue Data Structure using Linked List
 * http://www.sanfoundry.com/c-program-queue-using-linked-list/
 */
#include "webclient.h"
//#include <stdio.h>
//#include <stdlib.h>

struct timeval elasped;

struct nodeMetrics
{
	unsigned long perReqElapsed;
    struct nodeMetrics *ptr;
}*frontMetrics,*rearMetrics,*tempMetrics,*front1Metrics;

struct node
{
	//clientConn_t *conn;
	//int socket;
	char *fName;
    struct node *ptr;
}*front,*rear,*temp,*front1;


int count = 0;
int countMetrics = 0;

/********************
 * Metrics Queue Funcs
 */
void createQMetrics()
{
    frontMetrics = rearMetrics = NULL;
    printf("--> Metrics queue initialized ... \n");
}

void enQMetrics(unsigned long timeElapsed)
{
	if (rearMetrics == NULL)
    {
		rearMetrics = (struct nodeMetrics *)malloc(1*sizeof(struct nodeMetrics));
		rearMetrics->ptr = NULL;
		rearMetrics->perReqElapsed = timeElapsed;
        frontMetrics = rearMetrics;
    }
    else
    {
        tempMetrics =(struct nodeMetrics *)malloc(1*sizeof(struct nodeMetrics));
        rearMetrics->ptr = tempMetrics;
        tempMetrics->perReqElapsed = timeElapsed;
        tempMetrics->ptr = NULL;
        rearMetrics = tempMetrics;
    }
    printf("DEBUG:[%010u] New Q entry:[%lu]\n", pthread_self(), timeElapsed);
    countMetrics++;

}

/* Dequeing the queue */
unsigned long deQMetrics()
{
	long unsigned timeElapsed;
	front1Metrics = frontMetrics;

	if (front1Metrics == NULL)
	{
		printf("\n Error: Trying to display elements from empty queue");
		return(0);
	}
	else
		if (front1Metrics->ptr != NULL)
		{
			front1Metrics = front1Metrics->ptr;
			printf("INFO:[%010u] Dequed value : [%lu]\n", pthread_self(), frontMetrics->perReqElapsed);
			timeElapsed = frontMetrics->perReqElapsed;
			free(frontMetrics);
			frontMetrics = front1Metrics;
		}
		else
		{
			printf("INFO:[%010u] Dequed value : [%lu]\n", pthread_self(), frontMetrics->perReqElapsed);
			timeElapsed = frontMetrics->perReqElapsed;
			free(frontMetrics);
			frontMetrics = NULL;
			rearMetrics = NULL;
		}
		countMetrics--;
		return timeElapsed;
}

int emptyQMetrics()
{
     if ((frontMetrics == NULL) && (rearMetrics == NULL))
     {
    	 //printf("Queue empty ");
    	 return(1);
     }

    else
       //printf("Queue not empty ");

    return (0);

}

/*************************************
 * Worker thread filename queue funcs
 */

/* Create an empty queue */
void createQ()
{
    front = rear = NULL;
    printf("--> Queue initialized ... \n");
}



/* Returns queue size */
int sizeQ()
{
    printf("Queue size : %d\n", count);
    return (count);
}

/* Enqueing the queue */
void enQ(char *newFileName)

{
	//char *fName;
	//fName = strdup(newFileName);

	if (rear == NULL)
    {
        rear = (struct node *)malloc(1*sizeof(struct node));
        rear->ptr = NULL;
        rear->fName = strdup(newFileName);
        front = rear;
    }
    else
    {
        temp=(struct node *)malloc(1*sizeof(struct node));
        rear->ptr = temp;
        temp->fName = strdup(newFileName);
        temp->ptr = NULL;
        rear = temp;
    }
    printf("DEBUG:[%010u] New Q entry:[%s]\n", pthread_self(), newFileName);
    count++;
    //displayQ();
}


/* Displaying the queue elements */
void displayQ()
{
    front1 = front;

    if ((front1 == NULL) && (rear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (front1 != rear)
    {
        printf("[%s]", front1->fName);
        front1 = front1->ptr;
    }
    if (front1 == rear)
        printf("Front [%s]", front1->fName);
    printf("\n");


}


/* Dequeing the queue */
int deQ(char *fileName)
{
    //char *fileName;
	front1 = front;

    if (front1 == NULL)
    {
        printf("\n Error: Trying to display elements from empty queue");
        return(0);
    }
    else
        if (front1->ptr != NULL)
        {
            front1 = front1->ptr;
            printf("INFO:[%010u] deQ value[%s]\n", pthread_self(), front->fName);
            strcpy(fileName,front->fName);
            free(front->fName);
            free(front);
            front = front1;
        }
        else
        {
            printf("INFO:[%010u] deQ value[%s]\n", pthread_self(), front->fName);
            strcpy(fileName,front->fName);
            free(front->fName);
            free(front);
            front = NULL;
            rear = NULL;
        }
        count--;
        return 0;
}

/* Returns the front element of queue */
char * frontelement()
{
    if ((front != NULL) && (rear != NULL))
        return(front->fName);
    else
        return 0;
}

/* Display if queue is empty or not */
int emptyQ()
{
     if ((front == NULL) && (rear == NULL))
     {
    	 //printf("Queue empty ");
    	 return(1);
     }

    else
       //printf("Queue not empty ");

    return (0);

}


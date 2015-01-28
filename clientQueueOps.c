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
#include <stdio.h>
#include <stdlib.h>

struct node
{
	//clientConn_t *conn;
	int socket;
    struct node *ptr;
}*front,*rear,*temp,*front1;



int count = 0;


/* Create an empty queue */
void createQ()
{
    front = rear = NULL;
    printf("--> Queue initialized ... \n");
}

/* Returns queue size */
int sizeQ()
{
    printf("\n Queue size : %d", count);
    return (count);
}

/* Enqueing the queue */
void enQ(int newSocket)
{
    if (rear == NULL)
    {
        rear = (struct node *)malloc(1*sizeof(struct node));
        rear->ptr = NULL;
        rear->socket = newSocket;
        front = rear;
    }
    else
    {
        temp=(struct node *)malloc(1*sizeof(struct node));
        rear->ptr = temp;
        temp->socket = newSocket;
        temp->ptr = NULL;
        rear = temp;
    }
    //printf("DEBUG: --> New socket [%d] added to Q\n", newSocket);
    count++;
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
        printf("[%d]", front1->socket);
        front1 = front1->ptr;
    }
    if (front1 == rear)
        printf("[%d]", front1->socket);
    printf("\n");
}

/* Dequeing the queue */
int deQ()
{
    int retSocket;
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
            //printf("\n Dequed value : %d\n", front->socket);
            retSocket =front->socket;
            free(front);
            front = front1;
        }
        else
        {
           // printf("\n Dequed value : %d\n", front->socket);
            retSocket =front->socket;
            free(front);
            front = NULL;
            rear = NULL;
        }
        count--;
        return (retSocket);
}

/* Returns the front element of queue */
int frontelement()
{
    if ((front != NULL) && (rear != NULL))
        return(front->socket);
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


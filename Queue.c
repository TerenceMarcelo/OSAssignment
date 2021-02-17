/* Terence Marcelo 19163785
 * Date Created: 11/10/2018
 * Date Last Modified: 18/5/2020
 * OS Assignmnent Semester 1 2020
 *
 *
 *
 * This file has previously been sumitted as part of the UCP assignment in the 
 * previous semester as 'LinkedList.c' and has been modified for this 
 * assignment.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Queue.h"

Queue* newQueue( int sizeOfBuffer )
{
   /*Creates a queue*/
   Queue* newQueue;
   newQueue = (Queue*)malloc(sizeof(Queue));
   (*newQueue).head = NULL;
   (*newQueue).tail = NULL;
   (*newQueue).sizeOfBuffer = sizeOfBuffer;
   (*newQueue).total = 0;
   return newQueue;
}

void insert( struct Queue* inQueue, int inSource, int inDestination )
{
   /*Creates a struct with the value*/
   QueueNode* newNode;

   newNode = (QueueNode*)malloc(sizeof(QueueNode));
   newNode->source = inSource;
   newNode->destination = inDestination;
   newNode->next = NULL;

   if( inQueue->total < inQueue->sizeOfBuffer )
   {
      if( inQueue->head == NULL )
      {
      inQueue->head = newNode;
      inQueue->tail = newNode;
      }
      else
      {
         inQueue->tail->next = newNode;
         inQueue->tail = newNode;
      }
      inQueue->total = inQueue->total + 1;
   }
}

QueueNode* deQueue( struct Queue* inQueue )
{
   QueueNode* head = inQueue->head;
   QueueNode* temp;

   temp = head;

   if( inQueue->head->next != NULL )
   {
      inQueue->head = inQueue->head->next;
   }
   else
   {
      inQueue->head = NULL;
   }
   inQueue->total = inQueue->total - 1;

   return temp;
}

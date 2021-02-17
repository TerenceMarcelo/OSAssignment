#ifndef NODE_H
#define NODE_H
typedef struct QueueNode
{
   int source;
   int destination;
   struct QueueNode* next;
} QueueNode;
#endif

#ifndef HEAD_H
#define HEAD_H
typedef struct Queue
{
   QueueNode* head;
   QueueNode* tail;
   int sizeOfBuffer;
   int total;
} Queue;
#endif

#ifndef QUEUE_H
#define QUEUE_H

/* Returns a Linked List that it created. */
Queue* newQueue( int sizeOfBuffer );

/* Puts nValue into a struct and adds it to the last of
 * the list. */
void insert( struct Queue* queue, int source, int destination );
QueueNode* deQueue( struct Queue* inQueue );
#endif

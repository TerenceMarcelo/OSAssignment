/* Name: Terence Marcelo 19163785
 * Date Created: 16/10/2018
 * Date Last Modified: 18/5/2020
 * OS Assignment Semester 1 2020
 *
 *
 * This file has previously been used for the UCP Assignment in the previous
 * semester as 'Files.c' and has been modified for this assignment. */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include "lift_sim_B.h"
#include "Queue.h"

sem_t notEmpty;
sem_t notFull;
sem_t pmut;
sem_t cmut;

int main( int argsNo, char** args )
{
   if( argsNo < 3 )
   {
      printf( "\nPlease specify buffer size and time required.");
   }
   else
   {
      FILE* sim_input = fopen( "sim_input", "r" );
      FILE* sim_output = fopen( "sim_output", "a" );
      int totalSoFar = 0;  
      /* totalSoFar: Incremented everytime something added to buffer */
      Queue* buffer = newQueue( atoi( args[1] ) );
      struct filesAndBuffer threadArgs;
      /*int shm_id;*/
      pid_t pid;

      if( sim_input == NULL || sim_output == NULL )
      {
         printf( "\nsim_input or sim_output file is missing.\n");
         printf( "Please create those files first.\n\n\n");
      }
      else
      {
         /* Arguments to pass to each thread:   */
         /* - Pointers to both files, buffer, and total items to buffer so far */
         /* - Integer of how long a lift should take */
         threadArgs.sim_input = sim_input;
         threadArgs.sim_output = sim_output;
         threadArgs.buffer = buffer;
         threadArgs.totalSoFar = &totalSoFar;
         threadArgs.liftTime = atoi( args[2] );

         /*shm_id = shmget(2009, 2048, 0);*/
         sem_init( &notEmpty, 4, 1 );
         sem_init( &notFull, 4, 1 );
         sem_init( &cmut, 4, 1 );
         sem_init( &pmut, 4, 1 );

         printf( "\n\nNow processing. Please Wait.\n\n" );

         pid = fork();

         if( pid != 0 )
         {
            request( threadArgs );
         }
         else
         {
            pid = fork();
            if( pid == 0 )
            {
               fork();
            }
            printf( "\nHELLO\n" );
            lift( threadArgs );
         }
         sem_destroy( &notEmpty );
         sem_destroy( &notFull );
      }

      free( buffer );
      fclose( sim_input );
      fclose( sim_output );
   }

   printf( "\n\n" );
   return 0;
}

void* request( struct filesAndBuffer threadArgs )
{
   int nRead, source, destination;
   FILE* sim_input = threadArgs.sim_input;
   FILE* sim_output = threadArgs.sim_output;
   Queue* buffer = threadArgs.buffer;

   while( !feof( sim_input ) )
   {
      sem_wait( &pmut );
      while( buffer->total == buffer->sizeOfBuffer )
      {
         /* Wait first if buffer is still full */
         sem_post( &notFull );
      }

      nRead = fscanf( sim_input, "%d %d\n", &source, &destination );
      if( nRead == 2 )
      {
         /* Adds to buffer, prints to file, increments total so far, and */
         /* signals that the buffer isn't empty.   */
         insert( buffer, source, destination );
         ( *threadArgs.totalSoFar )++;
         fprintf( sim_output, "\n\n_____________________________________" );
         fprintf( sim_output, "\nNew Lift Request from %d to %d",
                                                         source, destination);
         fprintf( sim_output, "\nRequest No:%d", *threadArgs.totalSoFar );
         fprintf( sim_output, "\n_____________________________________\n\n" );
      }
      else if( nRead != 2 )
      {
         /* Program continues even if there's an invalid line in the file. */
         printf( "Invalid line detected. Line has been ignored." );
      }
      sem_post( &notEmpty );
      sem_post( &pmut );
   }
   
   printf( "\n\nAlmost Done.\n\n" );

   return NULL;
}

void* lift( struct filesAndBuffer threadArgs )
{
   QueueNode* nextToDo;
   FILE* sim_input = threadArgs.sim_input;
   FILE* sim_output = threadArgs.sim_output;
   Queue* buffer = threadArgs.buffer;
   int currentPosition = 1, requestsReceived = 0;
   int totalMovement = 0, currentMovement = 0;
   /* Integers above are just for the numbers that need to be written to file */
   /* Elevators start at 1 so currentPosition is initialised to 1   */

   while( buffer->total != 0 || !feof( sim_input ) )
   {
      sem_wait( &cmut );
      /* When there are items in the buffer or there will be in the future */
      while( buffer->total == 0 && !feof( sim_input ) )
      {
         /* Wait when there aren't any items in the buffer but */
         /* there will be in the future.  */
         sem_post( &notEmpty );
      }
      if( buffer->total != 0 )
      {
         nextToDo = deQueue( buffer );
         requestsReceived++;
         currentMovement = ( abs( currentPosition - nextToDo->source ) ) + 
                        abs( ( nextToDo->destination - nextToDo->source ) );
         totalMovement = totalMovement + currentMovement;
         fprintf( sim_output, "\n\nLift Operation" );
         fprintf( sim_output, "\nPrevious Position:%d",currentPosition );
         fprintf( sim_output, "\nRequest:Floor %d to Floor %d",
                                    nextToDo->source, nextToDo->destination );
         fprintf( sim_output, "\nDetails:" );
         fprintf( sim_output, "\n   Go from floor %d to floor %d",
                                          currentPosition, nextToDo->source );
         fprintf( sim_output, "\n   Go from floor %d to floor %d", 
                                    nextToDo->source, nextToDo->destination );
         fprintf( sim_output, "\n   #Movement for this request:%d",
                                                            currentMovement );
         fprintf( sim_output, "\n   #Request:%d", requestsReceived );
         fprintf( sim_output, "\n   Total #Movement:%d", totalMovement );
         currentPosition = nextToDo->destination;
         fprintf( sim_output, "\nCurrent Position:%d\n\n", currentPosition );
 
         sleep( threadArgs.liftTime ); 
         /* Send signal that there's now one less item in buffer. */
      }
      sem_post( &cmut );
      sem_post( &notFull );
   }

   return NULL;
}

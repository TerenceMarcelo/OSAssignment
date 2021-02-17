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
#include "lift_sim_A.h"
#include "Queue.h"

pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;

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

      pthread_t Lift_R;
      pthread_t Lift_1;
      pthread_t Lift_2;
      pthread_t Lift_3;

   
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
         /* - Integer for wether a line in the file is wrong */
         threadArgs.sim_input = sim_input;
         threadArgs.sim_output = sim_output;
         threadArgs.buffer = buffer;
         threadArgs.totalSoFar = &totalSoFar;
         threadArgs.liftTime = atoi( args[2] );
         threadArgs.fileErrors = 0;

         printf( "\n\nNow processing. Please Wait.\n\n" );
         
         pthread_create( &Lift_R, NULL, request, (void*)&threadArgs );
         pthread_create( &Lift_1, NULL, lift, (void*)&threadArgs );
         pthread_create( &Lift_2, NULL, lift, (void*)&threadArgs );
         pthread_create( &Lift_3, NULL, lift, (void*)&threadArgs );
         pthread_join( Lift_R, NULL );
         pthread_join( Lift_1, NULL );
         pthread_join( Lift_2, NULL );
         pthread_join( Lift_3, NULL );

      }

      free( buffer );
      fclose( sim_input );
      fclose( sim_output );
   }

   printf( "\n\n" );
   return 0;
}

void* request( void* inThreadArgs )
{
   int nRead, source, destination;
   struct filesAndBuffer* threadArgs = ( struct filesAndBuffer* )inThreadArgs;
   FILE* sim_input = threadArgs->sim_input;
   FILE* sim_output = threadArgs->sim_output;
   Queue* buffer = threadArgs->buffer;

   
   while( !feof( sim_input ) )
   {
      pthread_mutex_lock( &bufferMutex );
      while( buffer->total == buffer->sizeOfBuffer )
      {
         /* Wait first if buffer is still full */
         pthread_cond_wait( &notFull, &bufferMutex );
      }

      nRead = fscanf( sim_input, "%d %d\n", &source, &destination );
      if( nRead == 2 )
      {
         /* Adds to buffer, prints to file, increments total so far, and */
         /* signals that the buffer isn't empty.   */
         insert( buffer, source, destination );
         ( *threadArgs->totalSoFar )++;
         fprintf( sim_output, "\n\n_____________________________________" );
         fprintf( sim_output, "\nNew Lift Request from %d to %d",
                                                         source, destination);
         fprintf( sim_output, "\nRequest No:%d", *threadArgs->totalSoFar );
         fprintf( sim_output, "\n_____________________________________\n\n" );
         pthread_cond_signal( &notEmpty );
      }
      else if( nRead != 2 )
      {
         threadArgs->fileErrors = 1;
         /*Program doesn't continue if there's an invalid line in the file. */
         printf( "\n\nInvalid line detected." );
         printf( "\nCheck 'sim_input' and try again." );
      }
      pthread_mutex_unlock( &bufferMutex );
   }
   

   return NULL;
}

void* lift( void* inThreadArgs )
{
   QueueNode* nextToDo;
   struct filesAndBuffer* threadArgs = ( struct filesAndBuffer* )inThreadArgs;
   FILE* sim_input = threadArgs->sim_input;
   FILE* sim_output = threadArgs->sim_output;
   Queue* buffer = threadArgs->buffer;
   int currentPosition = 1, requestsReceived = 0;
   int totalMovement = 0, currentMovement = 0;
   /* Integers above are just for the numbers that need to be written to file */
   /* Elevators start at 1 so currentPosition is initialised to 1   */

   
   while( ( buffer->total != 0 || !feof( sim_input ) ) 
                                             && threadArgs->fileErrors != 1 )
   {
      /* When there are items in the buffer or there will be in the */
      /* future and that there hasn't been any bad lines in the file.*/
      pthread_mutex_lock( &bufferMutex ); 
      while( buffer->total == 0 && !feof( sim_input ) )
      {
         pthread_cond_wait( &notEmpty, &bufferMutex );
         /* Wait when there aren't any items in the buffer but */
         /* there will be in the future.  */
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

         free( nextToDo ); 
         sleep( threadArgs->liftTime ); 
         pthread_cond_signal( &notFull );
         /* Send signal that there's now one less item in buffer. */
      }
      pthread_mutex_unlock( &bufferMutex );
   }

   return NULL;
}

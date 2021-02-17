#include "Queue.h"
struct filesAndBuffer
{
   FILE* sim_input;
   FILE* sim_output;
   Queue* buffer;
   int* totalSoFar;
   int liftTime;
};
void* request( struct filesAndBuffer threadArgs );
void* lift( struct filesAndBuffer threadArgs );

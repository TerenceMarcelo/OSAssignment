#include "Queue.h"
struct filesAndBuffer
{
   FILE* sim_input;
   FILE* sim_output;
   Queue* buffer;
   int* totalSoFar;
   int liftTime;
   int fileErrors;
};
void* request( void* inThreadArgs );
void* lift( void* inThreadArgs );

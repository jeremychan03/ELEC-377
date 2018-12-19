/* 	
 * ELEC 377, Lab 3
 */

// The total size of the struct (all data in it)
#define MEMSIZE 200
// The size of the buffer (data pipe)
#define BUFFSIZE 5
// The maximum number of producer and consumer processes for the pass-the-key algorithm
#define NUMPROCS 5

// The structure of the shared memory
struct shared {

   // Sync varialbles
   int waiting[NUMPROCS];
   int lock;
   
   // Current number of active producers
   int numProducers;
   
   // Queue variables
   char buffer[BUFFSIZE];
   int in;
   int out;
   int count;

};

// Function prototypes
void mutexInit(struct shared *memptr);
void getMutex(short pid);
void releaseMutex(short pid);

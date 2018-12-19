/* 	
 * ELEC 377, Lab 3
 */

// External libraries
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>

// Shared data and pass the key alg.
#include "common.h"

// For redibility
#define FALSE 0
#define TRUE 1

#define MYPID 1

int main (int argc, char *argv[]){

	// initialize the shared memory, load in the initial array's, spawn the worker
	// processes.

	key_t   key;
	struct shared    *memptr;
	int shmid;
	int c,stored;
    
   printf("Starting...\n");
   
    if (argc != 2) {
        fprintf(stderr,"Usage: %s processid\n",argv[0]);
        exit(1);
    }
    // no error checking...
    int pid = atoi(argv[1]);
    if (pid < 0 || pid >= NUMPROCS){
        fprintf(stderr,"pid between 0 and %d\n",NUMPROCS-1);
        exit(1);
    }

   printf("Got pid: %d\n", pid);
   
	/*	 Shared memory init 	*/
	key = ftok(".", 'S');
	if((shmid = shmget(key, MEMSIZE, IPC_CREAT|0666)) == -1 ){
		if( (shmid = shmget(key, MEMSIZE, 0)) == -1){
			printf("Error allocating shared memory. \n");
			exit(1);
		}
	}

	// now map the region..
	if((int)(memptr = (struct shared *) shmat(shmid, 0, 0)) == -1){
	    printf("Couldn't map the memory into our process space.\n");
	    exit(1);
	}
	
   //printf("Initializing memory\n");
   
   // Initialize shared memory pointed
   mutexInit(memptr);

   //printf("Entering the reading loop\n");
   
   // Increment the number of producers in a critical section
   getMutex(pid);
   memptr->numProducers++;
   releaseMutex(pid);
   
   //printf("Incremented numproducers\n");
   
   // While there ase still characters in the standard input, read them
   while((c = getchar()) != EOF){
      
      //printf("Got character: %c (%d)\n", c, c);
      
     // Has the character been put in the buffer?
     stored = FALSE;
     
     // While the character was not put in the buffer
     while(!stored){
         
     // Enter the critical section
	 getMutex(pid);
	 
	 //printf("Putting %c in buffer, current count: %d\n", c, memptr->count);
	 
	 // If there is space in the buffer
	 if(memptr->count < BUFFSIZE){
	    // Put the character in the buffer
	    memptr->buffer[memptr->in] = c;
	    // Debug output
	    printf("Pid %d put %c in index [%d], count is now: %d\n", pid, c, memptr->in, (memptr->count + 1));
	    // Set the index of where to put the next character
	    memptr->in = (memptr->in + 1) % BUFFSIZE;
	    // Increment the number of characters in the buffer
	    memptr->count++;
	    // The character has been stored
	    stored = TRUE;
	 }
	 
	 // Exit the critical section
	 releaseMutex(pid);
      }
      //*/
   }
   
   //printf("Got all data\n");
   
   // After all the data has been put in the buffer, decrement the number of producers in a critical section
   getMutex(pid);
   memptr->numProducers--;
   releaseMutex(pid);
   
   // Return from main
   return 0;
}



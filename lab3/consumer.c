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

// Shared memory and pass the key alg.
#include "common.h"

// For readibility
#define FALSE 0
#define TRUE 1

#define MYPID 0

int main (int argc, char *argv[]){

	// initialize the shared memory, load in the initial array's, spawn the worker
	// processes.

	key_t   key;
	struct shared    *memptr;
	int shmid;
    
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
	
	// Initialize the shared memory pointer
	mutexInit(memptr);
	
   //printf("Consumer %d initialized\n", pid);
   
   // The current character to store
   char c;
   // It is the end of the data?
   int isEnd = FALSE;   
   
   // While there is more data
   while(!isEnd){
      
      //printf("Getting more data...\n");
      
      // Was the data read from the buffer?
      int read = FALSE;
      
      // While the data was not read from the shared buffer
      while(!read){
          
      // Enter the critical section
	  getMutex(pid);
	 
	 //printf("Trying to read from buffer, size is %d, %d producers\n", memptr->count, memptr->numProducers);
	 
	 // If there is data in the buffer
	 if(memptr->count > 0){
	    // Read a character from the buffer
	    c =  memptr->buffer[memptr->out];
	    //printf("Got character: %c (%d) at index %d\n", c, c, memptr->out);
	    // Indicate the index of the next character
	    memptr->out = (memptr->out + 1) % BUFFSIZE;
	    // Decrement the # of characters in the buffer
	    memptr->count--;
	    // The data was read
	    read = TRUE;
	 }
	 // The buffer is empty
	 else{
	    //printf("Buffer is empty, %d producers\n", memptr->numProducers);
	    
	    // If there are no more producers
	    if(memptr->numProducers == 0){
	       //printf("End\n");
	       // All the data was read
	       isEnd = TRUE;
	       read = TRUE;
	    }
	 }
	 
	 // Exit critical section
	 releaseMutex(pid);
	 
	 //printf("%c", c);
	 
     }
      
     // If the data is valid, print it out
     if(!isEnd){
    	//printf("---------- Printing out char: %c\n", c);
    	printf("%c", c);
    	fflush(stdout);
     }
      
      //*/
   }
   
   //printf("Done!\n");
    
	return 0;
}



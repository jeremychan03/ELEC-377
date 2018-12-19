/* 	
 * ELEC 377, Lab 3
 */

// Include external libraries
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <asm/system.h>

// Include shared data struct
#include "common.h"

// Because C does not have built-in T&F
#define FALSE 0
#define TRUE 1

// A pointer to the shared memory
static struct shared *sharedptr;

// The atomic test and set function which uses the compare and exchange assembly code macro
int test_and_set(int * lock){
    return __cmpxchg(lock,0,1,4);
}

// Set the shared memory pointer
void mutexInit(struct shared *memptr){
        // initialize the only mutex once, from the producer... 
    sharedptr = memptr;
}

// To enter critical section
void getMutex(short pid){
   
   // this should not return until it has mutual exclusion. Note that many versions of 
   // this will probobly be running at the same time.

   // Indicate that this process want the key
   int key = TRUE;
   sharedptr->waiting[pid] = TRUE;
   // Wait for the key to become available
   while(sharedptr->waiting[pid] && key){
      key = test_and_set(&(sharedptr->lock));
   }
   // Got the key, not waiting anymore
   sharedptr->waiting[pid] = FALSE;
   
}

// To exit critical section
void releaseMutex(short pid){
   
   // set the mutex back to initial state so that somebody else can claim it

   // Calculate the next pid
   int nextPid = (pid + 1) % NUMPROCS;
   // Loop though all the processes, as long as the current process is not waiting and it it not this process
   while(nextPid != pid && !sharedptr->waiting[nextPid]){
	nextPid = (nextPid + 1) % NUMPROCS;
   }
   // If no processes are waiting, release the key
   if(pid == nextPid){
	sharedptr->lock = FALSE;
   }
   // Otherwise, pass the key to the next running process
   else{
      sharedptr->waiting[nextPid] = FALSE;
   }
   
}

 

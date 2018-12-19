/*+
 * Module:  lab2.c
 *
 * Purpose: Skeleton solution to ELEC 377 Lab2.
 *
-*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#define NR_RUNNING_ADDRESS 0xc03dc3a4
#define NR_THREADS_ADDRESS 0xc038b3a8

static struct task_struct * firstTask, *lastTask;

int cnt;

int my_read_proc(char * page, char **start, off_t fpos, int blen, int * eof, void * data){

    int numChars;
    if (fpos == 0){
       // Get number of running processes & threads
       //int *nr_processes = (int*)NR_RUNNING_ADDRESS;
       int *nr_threads = (int*)NR_THREADS_ADDRESS;
       
        // write headers
	numChars = sprintf(page, "Number of running processes: %d\n", nr_running);
        numChars += sprintf(page + numChars, "Number of threads: %d\n", *nr_threads);
        numChars += sprintf(page + numChars, "%-5s%-5s%-5s\n", "PID", "UID" ,"NICE"); 
       
        // find first task
	firstTask = &init_task;
        lastTask = firstTask;
	 
        if(firstTask->pid != 0){	    
	   // write first task
	   numChars += sprintf(page + numChars, "%-5d%-5d%-5d\n", firstTask->pid, firstTask->uid, firstTask->nice);
	}
       
        // advance to next task
        firstTask = firstTask->next_task;	
       
    } else {
        if (firstTask == lastTask){
            *eof = 0;
            *start = page;
            return 0;
        }
       
        if(firstTask->pid != 0){
	   // write task info for one task
	   numChars = sprintf(page, "%-5d%-5d%-5d\n", firstTask->pid, firstTask->uid, firstTask->nice);
	}
       
        // advance to next task
        firstTask = firstTask->next_task;
    }
    *eof = 1;
    *start = page;
    return numChars;
}

int init_module(){
   struct proc_dir_entry * proc_entry;

   proc_entry = create_proc_entry("lab2", 0444, NULL);
   if(proc_entry == NULL) return -1;
   proc_entry->read_proc = my_read_proc;
   return 0;

}

void cleanup_module(){
   remove_proc_entry("lab2", NULL);
}

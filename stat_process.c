#include "helper.h"
#include "request.h"
#include <semaphore.h>

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <time.h>
#include <stdlib.h>
#include "slot.h"

int main(int argc, char *argv[])
{
  fprintf(stdout,"here");
  if(argc != 4) {
    exit(1);
  }
  if(atoi(argv[2]) < 0) {
    exit(1);
  }
  if(atoi(argv[3]) < 0) {
    exit(1);
  }
  char* shm_name = argv[1];
  int pagesize = getpagesize();
  int newshmfd = shm_open(shm_name, O_RDWR, 0660);
  if(newshmfd < 0) {
    exit(1);
  }

  slot_t* newshm_ptr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, newshmfd,0);  
  
 
  int sleeptime_ms = atoi(argv[2]);
  int numthreads = numthreads = atoi(argv[3]);
  int sleeptime_sec = 0;
  struct timespec remaining, request = {sleeptime_sec,sleeptime_ms};
  
  
  //char* shm_name = argv[1];
  
  
  //

  //listenfd = Open_listenfd(port);
  fprintf(stdout, "hello");
  int counter = 0;
  while (1) {
    counter++;
    printf("%i\n",counter);
    nanosleep(&request,&remaining);
    for(int i = 0; i < numthreads; i++) {
      
      printf("%ld : %i %i %i\n",newshm_ptr[i].id,newshm_ptr[i].total,newshm_ptr[i].statics,newshm_ptr[i].dynamic); 

    }
    

  }
  
  
  
  
}

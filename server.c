#include "helper.h"
#include "request.h"
#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include "slot.h"

#define BUF_LEN 20
const int BUFFER_LEN = BUF_LEN;
static int size;
static int count;
static int* shared;
static pthread_cond_t full;
static pthread_cond_t empty;
static pthread_mutex_t lock;
static int isFull = 0;
int pagesize;
int shmfd;
int ret;
static int bufSize;
char* SHM_NAME;

//slot_t * shm_ptr;

void* produce( void* arg ) {
    pthread_mutex_lock(&lock);
    char* temp;
    
    while(1) {
        while (isFull) {
            pthread_cond_wait(&empty, &lock);
        }
        temp = readline("Type something: ");

        //we need space for a null terminating character
        //so strlen == BUFFER_LEN would result in one char getting chopped
        if (!temp) {
            shared[0] = EOF;
            isFull = 1;
            pthread_cond_signal(&full);
            pthread_mutex_unlock(&lock);
            return NULL;
       
        }

        if (strlen(temp) >= BUFFER_LEN) {
            fprintf(stderr, "Line was too long. Skipping.\n");
            free(temp);
            continue;
        }

        //strncpy(shared, temp, BUFFER_LEN);
        
        free(temp);

        isFull = 1;
        pthread_cond_signal(&full);
    }
    

    pthread_mutex_unlock(&lock);
    return NULL;

}

void* consume( void* arg ) {
    fprintf(stdout,"pthread_self: %ld\n", pthread_self());
    printf("work\n");
    
    //int pagesize = getpagesize();
    //slot_t * shm_ptr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    pthread_mutex_lock(&lock);
    while (1) {
        printf("count: %i\n",count);
        while (!isFull) {
            printf("stuck\n");
            pthread_cond_wait(&empty, &lock);
        }
        isFull = 0;
        //shm_ptr[count].id = (char*)pthread_self();
        //shm_ptr[count].totalNum++;
        
        
        
        
        
        shared[count-1] = 0;
        count--;
        printf("consuming\n");
        pthread_cond_signal(&full);
    }
    
    requestHandle(shared[count]);
    Close(shared[count]);
    pthread_mutex_unlock(&lock);
}


static void my_handler(int sig) {
    printf("captured signal %d\n",sig);
    //pagesize = getpagesize();
    //shmfd = shm_open("shrdregion", O_RDWR | O_CREAT, 0660);
    /*
    if(shmfd < 0) {
        perror("shm_open");
        exit(0);
    }
    */
    //shm_ptr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    /*
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }
    */
    int pagesize = getpagesize();
    int ret1 = munmap(shm_ptr, pagesize);
    
    if (ret1 != 0) {
      fprintf(stdout,"ret: %i\n",ret1);
      exit(1);
    }
    
    
    int ret2 = shm_unlink(SHM_NAME);
    
    if (ret2 != 0) {
        fprintf(stdout,"shm_unlink");
        exit(1);
    }
    
   exit(0);


}

// 
// server.c: A very, very simple web server
//
// To run: 
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

void getargs(int *port, int argc, char *argv[])
{
    
  //printf("hello\n");
  if (argc != 5) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }
  if(atoi(argv[1]) < 2000) {
      exit(1);
  }
  if(atoi(argv[2]) < 0) {
      exit(1);
  }
  if(atoi(argv[3]) < 0) {
      exit(1);
  }
  
  *port = atoi(argv[1]);
}


int main(int argc, char *argv[])
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  getargs(&port, argc, argv);

  //
  //

  
  int pagesize = getpagesize();
  SHM_NAME = argv[4];
  int shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
  //fprintf(stdout,"shm_open");
  if(shmfd < 0) {
      fprintf(stdout,"shm_open");
      return 1;
  }
  
  ftruncate(shmfd,pagesize);
  
  if(ftruncate(shmfd,pagesize)==-1) {
      fprintf(stdout,"ftruncate");
      return 1;
  }
  
  // Memory map.
  shm_ptr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
  
  if (shm_ptr == MAP_FAILED) {
      fprintf(stdout,"mmap");
        return 1;
  }
  
  //fprintf(stdout,"%s \n", shm_ptr[0].id);

  
  
     


  // 
  // CS537 (Part A): Create some threads...
  //
  
  pthread_mutex_init(&lock,NULL);
  pthread_cond_init(&full,NULL);
  pthread_cond_init(&empty,NULL);
  size = atoi(argv[2]);
  slot_size = size;
  bufSize = atoi(argv[3]);
  shared = malloc(bufSize*sizeof(int));
  //pthread_t workers[size];
  //pthread_t threads[size];
  for(int i = 0; i < size; ++i) {
      pthread_t p1;
      int *p = (int*)malloc(size*sizeof(pthread_t));
      *p = i;
      pthread_create(&p1,NULL,consume,(void*)p);
      //workers[i] = p1;
      shm_ptr[i].id = pthread_self();
      

      //printf("flag: %i\n", flag);
      //pthread_create(&c1,NULL,consume,NULL);
  }

/*
  for(int i = 0; i < size; ++i) {
      pthread_join(workers[i],NULL);
  }
  */
  
  

  listenfd = Open_listenfd(port);
  
  while (1) {
    signal(SIGINT, my_handler);
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    
    if (connfd != 0) {
        pthread_mutex_lock(&lock);
        while (count >= bufSize) {
            isFull = 1;
            printf("all full\n");
            pthread_cond_signal(&empty);
            pthread_cond_wait(&full, &lock);
        }   
        
        shared[count] = connfd;
        
        count++;
        
        printf("producing\n");
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);
            
    }

    // 
    // CS537 (Part A): In general, don't handle the request in the main thread.
    // Save the relevant info in a buffer and have one of the worker threads 
    // do the work. Also let the worker thread close the connection.
    // 
    requestHandle(connfd);
    Close(connfd);
    
  }
  
  pthread_cond_destroy(&full);
  pthread_cond_destroy(&empty);
  

}

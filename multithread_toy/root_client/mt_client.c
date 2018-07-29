#include <pthread.h>
#include "cl_utils.h"
#include "cl_task.h"

#define NUM_THREADS_DEFAULT 300

/*void ctrl_c_handler(int sig){
  close(cfd);
  exit(0);
}*/

int main(int argc, char *argv[]){
  int NUM_THREADS;

  time_t end;
  time_t start = time(NULL);
  time_t seconds = 60;

  end = start + seconds;

  printf("start time is : %s", ctime(&start));

  if (argc < 3){
    fprintf(stderr, "Server name and/or port missing.");
    exit(1);
  }
  else if (argc == 3)
    NUM_THREADS = NUM_THREADS_DEFAULT;
  else
    NUM_THREADS = atoi(argv[3]);

  //signal(SIGINT, ctrl_c_handler);


  int count_req[NUM_THREADS];
  double total_time[NUM_THREADS];

  for(int l=0; l<NUM_THREADS; l++){
    count_req[l] = 0;
  }

  for(int m=0; m<NUM_THREADS; m++){
    total_time[m] = 0.0;
  }

  //printf("count root address %p\n", count_req);
  int port = atoi(argv[2]);
  
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  struct cl_thread_data *args;
  pthread_t threads[NUM_THREADS];
  for (int i=0; i<NUM_THREADS; i++){
      args = (struct cl_thread_data*)malloc(sizeof(struct cl_thread_data));
      args->count = count_req+i;
      args->task = do_task;
      args->port = port;
      args->hostname = argv[1];
      args->start = start;
      args->end = end;
      args->total_time = total_time+i;
   // printf("count address %p\n", args->count);
    // args.count = count_req+i;
    // args.total_time = total_time+i;
    if(pthread_create(threads+i, &attr, hit_the_server, args) != 0)
      error("pthread_create");
  }
  pthread_attr_destroy(&attr);

  for (int j=0; j<NUM_THREADS; j++){
    pthread_join(threads[j], NULL);
    printf("Client #%02d dies.\n", j);
  }

  int total_count = 0;
  double response_time = 0.0;
  for(int k=0; k<NUM_THREADS; k++){
    printf("Count of Thread %d is %d\n", k, count_req[k] );
    total_count = total_count + count_req[k];
  }

  for(int n=0; n<NUM_THREADS; n++){
    printf("Total time taken by thread %d is %f\n", n, total_time[n] );
    response_time = response_time + total_time[n];
  }
  printf("Total request:%d\n",total_count);
  int throughput = total_count/60;
  printf("Throughput :%d\n",throughput);
  printf("Total respone time:%f\n",response_time/NUM_THREADS);
  return 0;
}


#include "cl_utils.h"


void *hit_the_server(void *_args){
  struct cl_thread_data *args = (struct cl_thread_data*)_args;
  args->task(args->port, args->hostname, args->start, args->end, args->count, args->total_time);
  free(args);
  return NULL;
}

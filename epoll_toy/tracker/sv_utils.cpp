#include "sv_utils.h"

void error(const std::string &msg){
  perror(msg.c_str());
  exit(1);
}

void* handle_client(void *_args){
 // printf("Recieve here-1\n");
  struct sv_thread_data *args = (struct sv_thread_data *)_args;
  int i = 1;
  while(i != 3){
  i = args->task(args->cfd, args->tracker_file_map, args->peer_info_map);
  }
  free(args->c_addr);
  if (close(args->cfd) < 0)
    error("on close of fd?");
  free(args);
  printf("Address freed:%p\n", args);
  //printf("Recieve here-2\n");
  return NULL;
}

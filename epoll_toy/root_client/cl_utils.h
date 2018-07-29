#ifndef TT_CLIENT_UTILS_H
#define TT_CLIENT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>

struct cl_thread_data{
  void (*task)(int ,char *, time_t, time_t, int *, double *);
  int port;
  char *hostname;
  time_t start;
  time_t end;
  int *count;
  double *total_time;
}; 
void *hit_the_server(void *args);
#endif

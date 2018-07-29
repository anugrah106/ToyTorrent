#ifndef TT_SERVER_UTILS_H
#define TT_SERVER_UTILS_H

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <strings.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include "store.h"
#include <pthread.h>

struct sv_thread_data{
  struct sockaddr_in *c_addr;
  int cfd;
  json *tracker_file_map, *peer_info_map;
  responseType (*task)(int cfd, json *tfm, json *pim);
};

extern pthread_mutex_t mutex_tfm, mutex_pim;

void error(const std::string &msg);
void* handle_client(void *args);
#endif

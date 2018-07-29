#ifndef TT_PEER_UTILS_H
#define TT_PEER_UTILS_H

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <strings.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "json.hpp"

#define NUM_FETCH_WORKERS 3
#define MAX_FETCH_THREADS 2
#define LISTEN_Q_SIZE 5
#define BLOCK_SIZE 512

extern pthread_mutex_t mutex_history;
extern sem_t fetch_counter;

using json = nlohmann::json;

struct peer_thread_data{
  struct sockaddr_in *c_addr, *tracker_addr;
  int cfd, my_port;
  void (*task)(int cfd, int my_port, struct sockaddr_in *tracker_addr, json *history, const char *name);
  json *history;
  const char *name;
};

void error(const std::string &msg);
void* handle_client(void *_args);
std::string talkWithTracker(struct sockaddr_in *s_addr, const std::string &request, bool read_response=true);
void peer_shutdown(int my_port, struct sockaddr_in *tracker_addr);
int sendDiscovery(const json &history, const char *name, double bandwidth, int my_port, struct sockaddr_in *tracker_addr);
json discover();
#endif

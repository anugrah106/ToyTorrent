#include "utils.h"
#include "peer_task.h"
#include <signal.h>

sem_t fetch_counter;
pthread_mutex_t mutex_history;

static int port;
static struct sockaddr_in tracker_addr;

void ctrl_c_handler(int sig){
  peer_shutdown(port, &tracker_addr);
  exit(0);
}

int main(int argc, char *argv[]){
  if (argc < 5){
    fprintf(stderr, "Args missing: my-port tracker-port name bandwidth\n");
    exit(1);
  }
  signal (SIGINT, ctrl_c_handler);

  sem_init(&fetch_counter, 0, MAX_FETCH_THREADS);
  pthread_mutex_init(&mutex_history, NULL);
  
  struct sockaddr_in s_addr;
  port = atoi(argv[1]);
  int tracker_port = atoi(argv[2]);
  const char *name = argv[3];
  double bandwidth = atof(argv[4]);
  
  bzero(&s_addr, sizeof(s_addr));
  s_addr.sin_addr.s_addr = INADDR_ANY;
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(port);

  bzero(&tracker_addr, sizeof(tracker_addr));
  tracker_addr.sin_addr.s_addr = INADDR_ANY;
  tracker_addr.sin_family = AF_INET;
  tracker_addr.sin_port = htons(tracker_port);

  int sfd, cfd;
  sfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (sfd == -1)
    error("FD failed.");

  if (bind(sfd, (struct sockaddr*) &s_addr, sizeof(struct sockaddr_in)) == -1)
    error("Couldn't bind.");
  
  if (listen(sfd, LISTEN_Q_SIZE) == -1)
    error("Can't listen.");
  printf("Listening on %s...\n", argv[1]);
  char host[NI_MAXHOST], service[NI_MAXSERV];
  socklen_t c_addr_length = sizeof(struct sockaddr_in);

  //initialisation start
  json history = discover();
  std::cout << "Found " << history.size() << " files." << std::endl;

  if (sendDiscovery(history, name, bandwidth, port, &tracker_addr) < 0)
    error("could not greet tracker :(");
  else
    std::cout << "sent discovery to tracker\n";
  // initialisation done
  
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  
  while(1){
    struct sockaddr_in *c_addr = (struct sockaddr_in*) malloc(sizeof (struct sockaddr_in));
    bzero(c_addr, sizeof(struct sockaddr_in));
    cfd = accept(sfd, (struct sockaddr*) c_addr, &c_addr_length);
    if (cfd == -1)
      error("Couldn't accept connection.");

    pthread_t tid;
    struct peer_thread_data *args = (struct peer_thread_data*) malloc(sizeof (struct peer_thread_data));
    args->c_addr = c_addr;
    args->tracker_addr = &tracker_addr;
    args->cfd = cfd;
    args->my_port = port;
    args->task = do_task;
    args->history = &history;
    args->name = name;
    // handle_client must free args.c_addr and args
    
    int rc = pthread_create(&tid, &attr, handle_client, args);
    if (rc == 0){
      getnameinfo((const struct sockaddr*) c_addr, sizeof(struct sockaddr_in),
                  host, NI_MAXHOST, service, NI_MAXSERV, 0);
      printf("TID %5x handling (%s:%s) on FD %02d.\n", (int)tid, host, service, cfd);
    }
    else 
      error("pthread_create");
  }
}

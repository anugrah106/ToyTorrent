#include "sv_utils.h"
#include "sv_task.h"
#include "store.h"

#define NUM_THREADS 30

pthread_mutex_t mutex_tfm, mutex_pim;

int main(int argc, char *argv[]){
  if (argc < 2){
    fprintf(stderr, "Port missing.");
    exit(1);
  }
  
  struct sockaddr_in s_addr;
  int port = atoi(argv[1]);
  bzero(&s_addr, sizeof(s_addr));
  s_addr.sin_addr.s_addr = INADDR_ANY;
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(port);
  long long qu = 10000000000000;
  int sfd, rc;
  int cfd;
  int count = 0, count1 = 0;
  sfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (sfd == -1)
    error("FD failed.");

  if (bind(sfd, (struct sockaddr*) &s_addr, sizeof(struct sockaddr_in)) == -1)
    error("Couldn't bind.");
  
  if (listen(sfd, qu) == -1)
    error("Can't listen.");
  printf("Listening on %s...\n", argv[1]);
  char host[NI_MAXHOST], service[NI_MAXSERV];
  socklen_t c_addr_length = sizeof(struct sockaddr_in);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_mutex_init(&mutex_tfm, NULL);
  pthread_mutex_init(&mutex_pim, NULL);

  json tfm = {}, pim = {};
  
  while(1){

    struct sockaddr_in *c_addr = (struct sockaddr_in*) malloc(sizeof (struct sockaddr_in));
    bzero(c_addr, sizeof(struct sockaddr_in));
    cfd = accept(sfd, (struct sockaddr*) c_addr, &c_addr_length);
    if (cfd == -1)
      error("Couldn't accept connection.");

    pthread_t tid;

    struct sv_thread_data *args = (struct sv_thread_data*) malloc(sizeof (struct sv_thread_data));
    args->task = do_task;
    args->tracker_file_map = &tfm;
    args->peer_info_map = &pim;
    args->c_addr = c_addr;
    args->cfd = cfd;
    // Fixing the following line
    // struct sv_thread_data args = {&c_addr, cfd, do_task};
    //printf("Address :%p\n", args);
    // handle_client must free args.c_addr and args
    rc = pthread_create(&tid, &attr, handle_client, args);

    if (rc == 0){
      getnameinfo((const struct sockaddr*) c_addr, sizeof(struct sockaddr_in),
                   host, NI_MAXHOST, service, NI_MAXSERV, 0);
     // printf("TID %5x handling (%s:%s) on FD %02d.\n", (int)tid, host, service, cfd);
    }
    else {
      error("pthread_create");
    }
  }
}

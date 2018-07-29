#include "cl_task.h"

void error(char *msg){
  perror(msg);
  //exit(1);
}

void do_task(int port, char *hostname, time_t start, time_t end, int *count, double *total_time){

  clock_t local_start, local_end;
    double cpu_time_used, local_total_time = 0.0;
  struct sockaddr_in s_addr;
  int sfd;
  int local_count = 0; 
  struct hostent *server;
  server = gethostbyname(hostname);
  if (server == NULL)
    error("No host");
  char buffer[256] = "{\"type\":\"fetch\",\"data\":\"findingfile.cpp\"}";
  char buffer1[256];
  //int sockoptval = 1;
  long long n;
  bzero(&s_addr, sizeof(struct sockaddr_in));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(port);
  bcopy(server->h_addr, &s_addr.sin_addr.s_addr, server->h_length);

  sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    error("Bad FD");

    if (connect(sfd, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0)
       error("Connection failed");
  
  while(start < end){
    
   local_start = clock();
    n = write(sfd, buffer, strlen(buffer));
    
    if (n < 0)
      error("write failed");
    else{
      //printf("sent: %lld\n ", n);
      bzero(buffer1, 256);
      n = read(sfd, buffer1, 255);
    
    local_end = clock();
    
    if (n < 0)
      error("read error");
     else{
      cpu_time_used = ((double) (local_end - local_start)) / CLOCKS_PER_SEC;
      //printf("Received: %lld \n",n);
      local_count++;
      local_total_time = local_total_time + cpu_time_used;

//        printf("Cpu time :%f \n",cpu_time_used);
      }
    start = time(NULL);
     }
  }
  
  *count = local_count;
  *total_time = local_total_time/(*count);
  printf("Total time :%f\n", *total_time);
//  printf("count :%d\n",*count);
  close(sfd);
   
  	
}

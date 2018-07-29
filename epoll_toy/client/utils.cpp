#include "utils.h"
#include <sys/stat.h>
#include <dirent.h>

void error(const std::string &msg){
  perror(msg.c_str());
  exit(1);
}

void* handle_client(void *_args){
  struct peer_thread_data *args = (struct peer_thread_data*) _args;
  args->task(args->cfd, args->my_port, args->tracker_addr, args->history, args->name);
  free(args->c_addr);
  if (close(args->cfd) < 0)
    error("on close of FD?");
  free(args);
  return  NULL;
}

std::string talkWithTracker(struct sockaddr_in *s_addr, const std::string &request, bool read_response){
  int tfd = socket(AF_INET, SOCK_STREAM, 0);
  if (tfd < 0)
    error("Bad FD");

  if (connect(tfd, (struct sockaddr*) s_addr, sizeof(struct sockaddr_in)) < 0)
    error("Connection failed");
  long long n = write(tfd, request.c_str(), request.length());
  if (n < 0)
    error("getPeerList write failed");

  std::string response = "";
  if (read_response){
    char buffer[256] = {0};
    while (true){
      n = read(tfd, buffer, 255);
      if (n < 0)
        error("getPeerList read failed.");
      else if (n < 255){
        response += buffer;
        break;
      }
      else{
        response += buffer;
        bzero(buffer, 256);
      }
    }
    close(tfd);
  }
  return response;
}

void peer_shutdown(int my_port, struct sockaddr_in *tracker_addr){
  std::string request = R"({"type":"bye","port":)";
  request += "\"" + std::to_string(my_port) + "\"}";
  talkWithTracker(tracker_addr, request, false);
  
}

int sendDiscovery(const json &history, const char *name, double bandwidth, int my_port, struct sockaddr_in *tracker_addr){
  json discovery = {{"type", "discovery"},
                    {"data", {{"discovery", history},
                              {"bandwidth", bandwidth},
                              {"name", name}}},
                    {"port", std::to_string(my_port)}};
  std::string response = talkWithTracker(tracker_addr, discovery.dump());
  json r = json::parse(response);
  try{
    if (r.at("status") == "sucess")
      return 0;
  } catch (std::out_of_range &e){
    std::cerr << "bad response format from tracker??\n";
  }
  return 1;
}

json discover(){
  char the_path[256];
  getcwd(the_path, 255);

  struct stat stat_buffer;
  json discovery;

  DIR *dir;
  std::string base_path(the_path);
  base_path.append( "/files" );
  struct dirent *ent;
  if ((dir = opendir (base_path.c_str())) != NULL) {
    /* traverse all the files and directories within base directory */
    while ((ent = readdir (dir)) != NULL) {
      if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
        std::string file_path(base_path);
        file_path.append("/").append( ent->d_name );
        std::string block_list = "";

        stat(file_path.c_str() , &stat_buffer);
        long long size = stat_buffer.st_size;
        long long blocks = (size == 0)? 0 : 1 + ((size - 1) / BLOCK_SIZE);
        for(int a = 0 ; a < blocks ; a++) block_list += "1";
        discovery[ent->d_name] = {{"name", ent->d_name},
                                  {"block_list", block_list},
                                  {"size", size},
                                  {"num_blocks", blocks}};
      }
  	}
  	closedir (dir);
  }
  else{
    discovery = R"({})"_json;
    if (mkdir(base_path.c_str(), 0755) < 0)
      error("base_dir creation failed");
  }
  return discovery;
}

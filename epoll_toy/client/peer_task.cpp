#include "peer_task.h"
#include "utils.h"
#include <exception>

void do_task(int cfd, int my_port, struct sockaddr_in *tracker_addr, json *history, const char *name){
  char buffer[256] = {0};
  long long n = read(cfd, buffer, 255);
  if (n < 0)
    error("read failed.");
  printf("read: %lld ", n);

  std::string query(buffer);
  std::string response;
  responseType status = serveRequest(cfd, my_port, tracker_addr, name, query, *history, response);
  std::cout << "NFS " << (status == RT_NULL) << (status == RT_FAIL) << (status == RT_SUCCESS) << std::endl;
  if (status != RT_NULL){
    std::cout << "R " << response << std::endl;
    n = write(cfd, response.c_str(), response.length());
    if (n < 0)
      error("write failed.");
    else
      printf("sent: %lld\n", n);
  }
}

std::string readBlock(const std::string &file_name, long long block){
  std::string file_path = "files/" + file_name;
  FILE *file = fopen(file_path.c_str(), "r");
  if (file == NULL)
    error("opening file for read");
  if (fseek(file, block*BLOCK_SIZE, SEEK_SET) != 0)
    error("seek file");
  char buffer[BLOCK_SIZE+1] = {0};
  int n = fread(buffer, 1, BLOCK_SIZE, file);
  std::string data(buffer);
  return data;
}

responseType give(const std::string &file_name, long long block, json &history, std::string &response){
  auto it = history.find(file_name);
  if (it == history.end()){
    std::cerr << "I don't have blocks for '" << file_name << "'\n";
    response = R"({"status":"fail", "reason":"no blocks"})";
    return RT_FAIL;
  }
  json &data = it.value();
  std::cout << data.dump(4) << std::endl;
  std::string block_list = data.at("block_list");
  if (block >= data["num_blocks"] || block_list.at(block) == '0'){
    std::cerr << "I don't have block #" << block << " for '" << file_name << "'\n";
    json j = {{"status", "fail"},
              {"reason", "no block"},
              {"block_list", block_list}};
    response = j.dump();
    return RT_FAIL;
  }
  json j = {{"status", "success"}, {"data", readBlock(file_name, block)}};
  response = j.dump();
  return RT_SUCCESS;
}

std::string getPeerList(int my_port, struct sockaddr_in *tracker_addr, const std::string &file_name){
  std::string request = R"({"type":"fetch","port":)";
  request += "\"" + std::to_string(my_port);
  request += "\",\"data\":\"" + file_name + "\"}";
  return talkWithTracker(tracker_addr, request);
}

responseType download(int my_port, struct sockaddr_in *tracker_addr, const std::string &file_name, std::string &response){
  json info = json::parse(getPeerList(my_port, tracker_addr, file_name));
  /*
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_t tids[NUM_FETCH_WORKERS];
  struct fetch_worker_data *args = (struct fetch_worker_data*) malloc(sizeof (struct fetch_worker_data));
  */  
  //join
  response = info.dump();
  if (sem_post(&fetch_counter) < 0)
    error("on sem_post!");
  return RT_SUCCESS;
}

responseType serveRequest(int cfd, int my_port, struct sockaddr_in *tracker_addr, const char *name, const std::string &query, json &history, std::string &response){
  try{
    json q = json::parse(query);
    std::string verb = q.at("type");

    std::cout << "Q " << query << std::endl;
    
    if (verb == "kill"){
      // acquire all locks, dump history and exit(0)
      peer_shutdown(my_port, tracker_addr);
    }
    else if (verb == "give"){
      json &j = q.at("data");
      std::string file_name = j.at("name");
      long long block = j.at("block");
      return give(file_name, block, history, response);
    }
    else if (verb == "download"){
      std::string file_name = q.at("data");
      if (sem_trywait(&fetch_counter) < 0){
        std::cerr << "All fetchers are busy :(" << std::endl;
        response = R"({"status":"fail", "reason":"all fetchers are busy"})";
        return RT_FAIL;
      }
      if (sem_wait(&fetch_counter) < 0)
        error("on sem_wait!");
      return download(my_port, tracker_addr, file_name, response);
    }
    else if (verb == "list"){
      pthread_mutex_lock(&mutex_history);
      response = history.dump();
      pthread_mutex_unlock(&mutex_history);
      return RT_SUCCESS;
    }
    else
      throw std::out_of_range("bad verb.");
  } catch (std::out_of_range &e){
    std::cerr << e.what() << std::endl;
    response = R"({"status": "fail", "reason": "bad_verb"})";
    return RT_FAIL;
  } catch (std::invalid_argument &e){
    std::cerr << e.what() << std::endl;
    response = R"({"status": "fail", "reason": "bad_request_format"})";
    return RT_FAIL;
  } catch (std::exception &e){
    std::cerr << "UNHANDLED EXCEPTION " << e.what() << std::endl;
    response = R"({"status": "fail", "reason": "bad"})";
    return RT_FAIL;
  }
  return RT_NULL;
}

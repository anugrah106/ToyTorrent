#include <exception>
#include "store.h"

enum infoValidity { IV_VALID, IV_INVALID, IV_NEW };
extern pthread_mutex_t mutex_pim, mutex_tfm;

// THREAD UNSAFE, exception safe
infoValidity valid (const json &tracker_file_map, const json &file_info) {
  try{
    auto it = tracker_file_map.find(file_info.at("name"));
    if (it != tracker_file_map.end()){
      auto &item = it.value();
      /*
        std::cout << it.value() << " WHAT "  << std::endl;
        std::cout << file_info << " WHAT "  << std::endl;
        std::cout << (it.value()["name"] == file_info.at("name")) << std::endl;
        std::cout << (it.value()["size"] == file_info.at("size")) << std::endl;
        std::cout << (it.value()["num_blocks"] == file_info.at("num_blocks")) << std::endl;
        std::cout << (file_info.at("block_list").get<std::string>().length() == file_info.at("num_blocks")) << std::endl;
      */
      if (item["name"] == file_info.at("name") &&
          item["size"] == file_info.at("size") &&
          item["num_blocks"] == file_info.at("num_blocks") &&
          file_info.at("block_list").get<std::string>().length() == file_info.at("num_blocks")){
        return IV_VALID;
      }
      else{
        return IV_INVALID;
      }
    }
    else{
      return IV_NEW;
    }
  } catch (std::exception &e){
    std::cerr << "INFO_VALIDITY FAILED " << e.what() << std::endl;
    return IV_INVALID;
  }
}

// exception safe
void insertPeer (json &tracker_file_map, json &peer_info_map, const json &peer_file_map, const std::string &peer_port, const json &peer_info){
  pthread_mutex_lock(&mutex_pim);
  peer_info_map[peer_port] = peer_info;
  pthread_mutex_unlock(&mutex_pim);

  pthread_mutex_lock(&mutex_tfm);
  for (auto &file_info : peer_file_map){
    infoValidity validity = valid (tracker_file_map, file_info);
    if (validity == IV_INVALID){
      std::cerr << "bad file data from " << peer_port << std::endl;
      continue;
    }
    std::string file_name = file_info["name"];
    json &j = tracker_file_map[file_name];
    if (validity == IV_NEW){
      std::cout << "New " << file_name << " from " << peer_port << std::endl;
      j["num_blocks"] = file_info["num_blocks"];
      j["size"] = file_info["size"];
      j["name"] = file_name;
    }
    j["peers"][peer_port] = file_info["block_list"];
  }
  pthread_mutex_unlock(&mutex_tfm);
}

// exception safe
void removePeer (json &tracker_file_map, json &peer_info_map, const std::string &peer_port){
  std::vector<std::string> reap_list;
  
  // remove peer's data from each file.
  pthread_mutex_lock(&mutex_tfm);
  {
    for (auto &tracker_info : tracker_file_map){
      tracker_info["peers"].erase(peer_port);
      // if no peers have this file, reap this file.
      if (tracker_info["peers"].size() < 1)
        reap_list.push_back(tracker_info["name"]);
    }
    // reap 'em.
    for (auto &file_to_reap : reap_list)
      tracker_file_map.erase(file_to_reap);
  }
  pthread_mutex_unlock(&mutex_tfm);

  // remove peer info (bandwidth, port, etc.)
  pthread_mutex_lock(&mutex_pim);
  peer_info_map.erase(peer_port);
  pthread_mutex_unlock(&mutex_pim);
}

// exception safe
responseType updateBlocks (json &tracker_file_map, const json &peer_info_map, const std::string &peer_port, const json &file_info, json &response){
  pthread_mutex_lock(&mutex_pim);
  auto it = peer_info_map.find(peer_port);
  if (it == peer_info_map.end()){
    pthread_mutex_unlock(&mutex_pim);
    response = R"({"status": "fail", "reason": "update_block from unknown peer"})"_json;
    return RT_FAIL;
  }
  else {
    pthread_mutex_unlock(&mutex_pim);
    
    pthread_mutex_lock(&mutex_tfm);
    infoValidity validity = valid(tracker_file_map, file_info);
    if (validity == IV_INVALID){
      pthread_mutex_unlock(&mutex_tfm);
      response = R"({"status": "fail", "reason": "update_block has bad_file_info"})"_json;
      return RT_FAIL;
    }
    const std::string &file_name = file_info["name"];
    if (validity == IV_NEW){
      tracker_file_map[file_name] = {{"name", file_name},
                                     {"num_blocks", file_info["num_blocks"]},
                                     {"size", file_info["size"]}};
    }
    tracker_file_map[file_name]["peers"][peer_port] = file_info["block_list"];

    pthread_mutex_unlock(&mutex_tfm);
    response = R"({"status": "success"})"_json;
    return RT_SUCCESS;
  }
}

// exception safe
json queryFile (const json &tracker_file_map, const json &peer_info_map, const std::string &file_name){
  json res;
  pthread_mutex_lock(&mutex_tfm);
  json::const_iterator file_info_it = tracker_file_map.find(file_name);
  if (file_info_it != tracker_file_map.end()){
    const json &j = file_info_it.value().at("peers");
    res["size"] = file_info_it.value().at("size");
    res["num_blocks"] = file_info_it.value().at("num_blocks");
    pthread_mutex_lock(&mutex_pim);
    for (json::const_iterator it=j.begin(); it!=j.end(); it++){
      res["peers"][it.key()] = { { "bandwidth", peer_info_map[it.key()]["bandwidth"] },
                                 { "name", peer_info_map[it.key()]["name"] },
                                 { "block_list", it.value() } };
    }
    pthread_mutex_unlock(&mutex_pim);
  }
  else{
    res = R"({})"_json;
  }
  pthread_mutex_unlock(&mutex_tfm);
  return res;
}

// exception safe
responseType serveQuery (json &tracker_file_map, json &peer_info_map, const std::string &query, json &response){
  try {
    json q = json::parse(query);
    std::string verb = q.at("type");
    if (verb == "fetch"){
      std::string file_name = q.at("data");
      response = queryFile(tracker_file_map, peer_info_map, file_name); // exception safe
      return RT_SUCCESS;
    }
    else if (verb == "discovery"){
      std::string peer_port = q.at("port");
      json &payload = q.at("data");
      json &peer_file_map = payload.at("discovery");
      json peer_info = {{"bandwidth", payload.at("bandwidth")},
                        {"name", payload.at("name")}};
      insertPeer(tracker_file_map, peer_info_map, peer_file_map, peer_port, peer_info);
      response = R"({"status": "success"})"_json;
      return RT_DIS;
    }
    else if (verb == "tfm"){
      response = tracker_file_map;
      return RT_SUCCESS;
    }
    else if (verb == "update"){
      std::string peer_port = q.at("port");
      json &payload = q.at("data");
      return updateBlocks(tracker_file_map, peer_info_map, peer_port, payload, response);
    }
    else if (verb == "bye"){
      std::string peer_port = q.at("port");
      removePeer(tracker_file_map, peer_info_map, peer_port);
      return RT_NULL;
    }
    else{
      throw std::out_of_range("Bad verb (" + verb + ") in request.");
    }
  } catch (std::out_of_range &e) {
    std::cerr << e.what() << std::endl;
    response = R"({"status": "fail","reason": "bad_verb_or_payload"})"_json;
    return RT_FAIL;
  } catch (std::domain_error &e) {
    std::cerr << e.what() << std::endl;
    response = R"({"status": "fail","reason": "bad_payload"})"_json;
    return RT_FAIL;
  } catch (std::invalid_argument &e){
    std::cerr << e.what() << std::endl;
    response = R"({"status": "fail","reason": "bad_request_format"})"_json;
    return RT_FAIL;
  }
  return RT_FAIL;
}

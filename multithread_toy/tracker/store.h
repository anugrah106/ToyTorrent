#ifndef TT_STORE_H
#define TT_STORE_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "json.hpp"

#define BLOCK_SIZE 512 // bytes
using json = nlohmann::json;

enum responseType { RT_NULL, RT_FAIL, RT_SUCCESS , RT_DIS};

void insertPeer (json &tracker_file_map, json &peer_info_map, const json &peer_file_map, const std::string &peer_port, const json &peer_info);

void removePeer (json &tracker_file_map, json &peer_info_map, const std::string &peer_port);

responseType updateBlocks (json &tracker_file_map, const json &peer_info_map, const std::string &peer_port, const json &file_info, json &response);

json queryFile (const json &tracker_file_map, const json &peer_info_map, const std::string &file_name);

responseType serveQuery (json &tracker_file_map, json &peer_info_map, const std::string &query, json &response);

#endif

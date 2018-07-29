#ifndef TT_PEER_TASK_H
#define TT_PEER_TASK_H

#include "utils.h"

enum responseType { RT_NULL, RT_FAIL, RT_SUCCESS };

responseType serveRequest(int cfd, int my_port, struct sockaddr_in *tracker_addr, const char *name, const std::string &query, json &history, std::string &response);
void do_task(int cfd, int my_port, struct sockaddr_in *tracker_addr, json *history, const char *name);

#endif

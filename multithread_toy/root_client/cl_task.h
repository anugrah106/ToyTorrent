#ifndef TT_CL_TASK_H
#define TT_CL_TASK_H

#include "cl_utils.h" 

void do_task(int port , char *hostname, time_t start, time_t end, int *count, double *total_time);
void error(char *msg );

#endif

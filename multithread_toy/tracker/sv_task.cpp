#include "sv_task.h"
#include "sv_utils.h"

responseType do_task (int cfd, json *tfm, json *pim){
  char buffer[256] = {0};
  int n;
  std::string query = "";
    while (true){
      n = read(cfd, buffer, 255);
      if (n < 0)
        error("read failed.");
      else if (n < 255){
        query += buffer;
        break;
      }
        else{
        query += buffer;
        bzero(buffer, 256);
      }
    }
  
    //printf("Received\n");
    //printf("Received \n");
    if (query.length() == 0){
      return RT_DIS;
      //exit(2);
    }
    json response;
    responseType status = serveQuery(*tfm, *pim, query, response);
  
    //std::cout << " FNS " << (status == RT_FAIL) << (status == RT_NULL) << (status == RT_SUCCESS);
    //std::cout << std::endl << "R " << response << std::endl;
    //std::cout << " TFM " << std::endl << *tfm << std::endl << std::endl;
    //if (status != RT_NULL){
    std::string response_str = response.dump();
   
    n = write(cfd, response_str.c_str(), response_str.length());

    if (n < 0)
      error("write failed.");
  return status;
}

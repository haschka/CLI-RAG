#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

#include "local_resolve.h"

char* local_resolve(char* hostname) {

  struct addrinfo *initial_result;
  struct addrinfo *result;

  struct sockaddr_in *four_address;
  struct sockaddr_in6 *six_address;

  char* ip_address = (char*)malloc(sizeof(char)*512);
  
  int retval = getaddrinfo(hostname,NULL,NULL,&result);

  if(result) {
    switch(result->ai_family) {
    case AF_INET:
      four_address = (struct sockaddr_in*)result->ai_addr;
      inet_ntop(AF_INET,&four_address->sin_addr,ip_address,512);
      break;
    case AF_INET6:
      six_address = (struct sockaddr_in6*)result->ai_addr;
      inet_ntop(AF_INET6,&six_address->sin6_addr,ip_address,512);
      break;
    }
    freeaddrinfo(result);
    return(ip_address);
  }
  free(ip_address);
  return(NULL);
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]) {

  /* VARIABLES */
  int sockfd;
  int new_sockfd;
  int size = 512;
  int port = 8888;
  unsigned int sockfd_size;
  struct sockaddr_in sockaddr_client; /* client addr */
  struct sockaddr_in sockaddr_server; /* server addr */

  /* CREATE SOCKET */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
    printf("[ERROR] Could not create socket\n");
    exit(1);
  } else {
    printf("[CLIENT] Socket created sucessfully\n");
  }

  /* INITIALISATION OF SOCKADDR_CLIENT STRUCTURE */
  bzero(&sockaddr_client, sizeof(sockaddr_client));
  sockaddr_client.sin_family = AF_INET; // Protocol
  sockaddr_client.sin_port = htons(port); // Port
  sockaddr_client.sin_addr.s_addr = INADDR_ANY; // Address

  /* BIND */
  if( bind(sockfd, (struct sockaddr*)&sockaddr_client, sizeof(struct sockaddr)) == -1 ) {
    printf("[ERROR] Failed to bind\n");
    exit(1);
  } else {
    printf("[SERVER] Bind success, port: %d, address: 127.0.0.1\n",port);
  }

  /* LISTEN */
  if(listen(sockfd,3) == -1) {
    printf("[ERROR] Failed to listen on port %d\n", port);
    exit(1);
  } else {
    printf ("[SERVER] Listening on port %d.\n", port);
  }

  sockfd_size = sizeof(struct sockaddr_in);
  while((new_sockfd = accept(sockfd, (struct sockaddr *)&sockaddr_client, &sockfd_size))) {

    printf("[SERVER] Connection accepted for client %d\n", new_sockfd);

  }

  if (new_sockfd < 0) {
      printf("[SERVER] Failed to accept connection\n");
      return 1;
  }

  close(sockfd);
  close(new_sockfd);

}

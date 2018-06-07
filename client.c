#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFSIZE 1024 /* default size */
#define PORT 8888 /* port number */

int main(int argc, char *argv[]) {

	/* VARIABLES */
	int sockfd; /* socket */
  int n;
	char buffer[BUFFSIZE]; /* Buffer who send file */
	struct sockaddr_in sockaddr_server; /* server addr */

	/* CREATE SOCKET */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("[ERROR] Could not create socket\n");
	} else {
    printf("[CLIENT] Socket created sucessfully\n");
  }

	/* INITIALISATION OF SOCKADDR_SERVER STRUCTURE */
  bzero(&sockaddr_server, sizeof(sockaddr_server));
	sockaddr_server.sin_family = AF_INET;
	sockaddr_server.sin_port = htons(PORT);
  sockaddr_server.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* CONNECTION TO SERVER */
	if (connect(sockfd, (struct sockaddr *)&sockaddr_server, sizeof(struct sockaddr)) == -1) {
    printf("[ERROR] Failed to connect to remote\n");
	} else {
    printf("[CLIENT] Connected to server on port %d\n\n", PORT);
  }

  while ((n = read(sockfd, buffer, sizeof(buffer)-1)) > 0) {
    buffer[n] = 0;
    fflush(stdin);
    printf("%s",buffer);
  }

  if(n < 0)
  {
      printf("\n [CLIENT] Read error \n");
  }

  printf("[CLIENT] Connection lost.\n");
  close (sockfd);
	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFSIZE 1024 /* default size */
#define PORT 8888 /* port number */

void *connection_handler(void *socket);

int main(int argc, char *argv[]) {

  /* VARIABLES */
  int sockfd;
  int new_sockfd;
  int* new_socket;
  unsigned int sockfd_size;
  struct sockaddr_in sockaddr_client; /* client addr */
  struct sockaddr_in sockaddr_server; /* server addr */
  char buffer[BUFFSIZE]; /* Buffer who received file */
  char* message;

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
  sockaddr_client.sin_port = htons(PORT); // Port
  sockaddr_client.sin_addr.s_addr = INADDR_ANY; // Address

  /* BIND */
  if( bind(sockfd, (struct sockaddr*)&sockaddr_client, sizeof(struct sockaddr)) == -1 ) {
    printf("[ERROR] Failed to bind\n");
    exit(1);
  } else {
    printf("[SERVER] Bind success, port: %d, address: 127.0.0.1\n",PORT);
  }

  /* LISTEN */
  if(listen(sockfd,3) == -1) {
    printf("[ERROR] Failed to listen on port %d\n", PORT);
    exit(1);
  } else {
    printf ("[SERVER] Listening on port %d.\n", PORT);
  }

  sockfd_size = sizeof(struct sockaddr_in);
  while((new_sockfd = accept(sockfd, (struct sockaddr *)&sockaddr_client, &sockfd_size))) {

    printf("[SERVER] Connection accepted for client %d\n", new_sockfd);

    //Thread creation
    pthread_t sniffer_thread;
    new_socket = malloc(1);
    *new_socket = new_sockfd;

    //Thread connection handler
    if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_socket) < 0){
        printf("[SERVER] Could not create thread\n");
        return 1;
    }

    //Now join the thread , so that we dont terminate before the thread
    //pthread_join( sniffer_thread , NULL);
    fflush(stdin);
    printf("[SERVER] Handler assigned to client %d\n", new_sockfd);

  }

  if (new_sockfd < 0) {
      printf("[SERVER] Failed to accept connection\n");
      return 1;
  }

  close(sockfd);
  close(new_sockfd);

}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket)
{
    //Get the socket descriptor
    int sock = *(int*)socket;
    int read_size;
    char *message;
    char client_message[BUFFSIZE];

    //Send some messages to the client
    message = "\n[SERVER] Greetings! I am your connection handler, this is to communicate with you...\n";
		if(write(sock , message , strlen(message)) < 0) {
				printf("[ERROR] Failed to sent message.\n");
		}

    //Free the socket pointer
    free(socket);

    return 0;
}

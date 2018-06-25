#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "tools/HTTPRequestManager.h"

#define BUFFSIZE 2048 /* default size */
#define PORT 8888 /* port number */

typedef enum { false = 0, true = !false } bool;

void *connection_handler(void *);
void responseTreatment(char* , void *);
bool receive_file(char*, char*);
bool sendFile(char* file_title, int sockfd);

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
    char msg[BUFFSIZE];
    char client_message[BUFFSIZE];

    //Send some messages to the client
    message = "\n[SERVER] Greetings! I am your connection handler, this is to communicate with you...\n";
		if(write(sock , message , strlen(message)) < 0) {
				printf("[ERROR] Failed to sent message.\n");
		}

    while( (read_size = read(sock , client_message , sizeof(client_message))) > 0 ) {

				if (sizeof(client_message) == 0) {
            printf("[SERVER] error message is empty\n");
						break ;
				}

        printf("\n[CLIENT] Response: %s\n|END OF RESPONSE|\n", client_message);

        //responseTreatment(client_message, socket);

        //printf("\n\nMMMMSSGGGGG %s\n\n", msg);
        //sprintf(msg, "[SERVER] I received : %s", client_message);
        //write(sock, msg, sizeof(msg));
    }

    if(read_size == 0) {
        printf("[CLIENT] Client disconnected\n");
        fflush(stdout);
    } else if(read_size == -1) {
        printf("[CLIENT] Read failed\n");
    }

    //Free the socket pointer
    free(socket);

    return 0;
}

void responseTreatment(char* response, void *socket) {

  char* message;
  int sock = *(int*)socket;

  /*printf("METHOD: %s\n", getRequestType(response));
  printf("TYPE: %s\n", getType(response));
  printf("FILE TITLE: %s\n", getFileTitle(response));
  printf("CONTENT: %s\n", getContent(response));*/

  if (strcmp(getRequestType(response), "POST") == 0) {

    if (strcmp(getType(response), "File") == 0) {
      bool isReceived = receive_file(getFileTitle(response), getContent(response));
      if (isReceived == true) {
        message = "[SERVER] File received\n";
        write(sock, message, strlen(message));
      }
    }

  } else if (strcmp(getRequestType(response), "GET") == 0) {
      if (strcmp(getType(response), "File") == 0) {
        printf("TEST");
        bool isSend = sendFile(getFileTitle(response), sock);
      }
  }

}

bool receive_file(char* file_title, char* file_content) {
  puts("[SERVER] RECEIVE FILE FUNCTION");

  //char* received_file = "received.txt";
  char* folder = "./received_files/";
  char* result_path = malloc(strlen(folder)+strlen(file_title)+1);
  strcpy(result_path, folder);
  strcat(result_path, file_title);

  FILE *rFile = fopen(result_path, "a");

  if(rFile == NULL) {
    printf("[ERROR] File %s can't be open by the server.\n", file_title);
    return false;
  } else {

      int rFile_size = strlen(file_content);

      int write = fwrite(file_content, sizeof(char), rFile_size, rFile);

      if(write < rFile_size) {
        printf("[ERROR] File write failed on server.\n");
      }

      if(rFile_size < 0) {
        printf("[ERROR] File not received.\n");
      } else {
        printf("[STATUS] Ok received file %s from client!\n", file_title);
      }

      fclose(rFile);
      return true;
    }

    return false;
}

bool sendFile(char* file_title, int sockfd) {

  printf("SEND FILE FUNCTION\n");

  char file_buffer[BUFFSIZE];
  //char* file_name = message;
  char* folder = "./received_files/";
  char* result_path = malloc(strlen(folder)+strlen(file_title)+1);
  strcpy(result_path, folder);
  strcat(result_path, file_title);
  printf("FILE PATH %s\n", result_path);

  printf("[CLIENT] Sending %s to the Client...\n", file_title);

  FILE *file_to_send = fopen(result_path, "r");
  if(file_to_send == NULL) {
    printf("[ERROR] File %s not found.\n", file_title);
    return false;
  }

  bzero(file_buffer, sizeof(file_buffer));
  int file_size;
  while((file_size = fread(file_buffer, sizeof(char), BUFFSIZE, file_to_send)) > 0) {
      //char* file_title;
      //sprintf(file_title, "%s", file_title);
      //sprintf(message, postFile, "File", file_title, file_buffer);
      //printf("MESSAGE %s\n", message);
      if(send(sockfd, file_buffer, strlen(file_buffer), 0) < 0) {
          printf("[ERROR] Failed to sent file %s.\n", file_title);
          break;
      }
      bzero(file_buffer, sizeof(file_buffer));

  }

  printf("[STATUS] Ok File %s from Client was Sent!\n", file_title);
  //message = "";
  return true;
}

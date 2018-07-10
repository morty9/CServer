#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "tools/HTTPRequestManager.h"

#define BUFFSIZE 4096 /* default size */
#define PORT 8888 /* port number */

#define CODE_OK = "200 OK"
#define CODE_CREATED = "201 CREATED"
#define CODE_MOVED = "301 MOVED PERMANENTLY"
#define CODE_FOUND = "302 FOUND"
#define CODE_BAD_REQUEST = "400 BAD REQUEST"
#define CODE_FORBIDDEN = "403 FORBIDDEN"
#define CODE_NOT_FOUND = "404 NOT FOUND"
#define CODE_NOT_ALLOWED = "405 METHOD NOT ALLOWED"
#define CODE_INTERNAL_ERROR = "500 INTERNAL ERROR"
#define CODE_BAD_GETAWAY = "502 BAD GATEWAY"
#define CODE_UNAVAILABLE = "503 SERVICE UNAVAILABLE"

typedef enum { false = 0, true = !false } bool;

void *connection_handler(void *);
void responseTreatment(char* , void *);
bool receive_file(char*, char*);
bool sendFile(char* file_title, void* sockfd);
//char* sendFile(char* file_title, void *socket);
void handlerSignal(int sig);

int sockfd;
int new_sockfd;

int main(int argc, char *argv[]) {

  /* VARIABLES */
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

    signal(SIGINT, handlerSignal);

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

  //free(new_socket);
  close(sockfd);
  close(new_sockfd);
}

void handlerSignal(int sig) {

  if (sig == SIGINT) {
    close(sockfd);
    close(new_sockfd);
    kill(getpid(), SIGKILL);
  }

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

    while( (read_size = read(sock , client_message , BUFFSIZE)) > 0 ) {

				if (strlen(client_message) == 0) {
            printf("[SERVER] error message is empty\n");
						break ;
				}

        printf("\n[SERVER] Client Response: %s\n|END OF RESPONSE|\n", client_message);

        responseTreatment(client_message, socket);

        //printf("\n\nMMMMSSGGGGG %s\n\n", msg);
        //sprintf(msg, "[SERVER] I received : %s", client_message);
        //write(sock, msg, sizeof(msg));
    }

    if(read_size == 0) {
        printf("[SERVER] Client disconnected\n");
        fflush(stdout);
    } else if(read_size == -1) {
        printf("[SERVER] Nothing to read\n");
    }

    //Free the socket pointer
    free(socket);
    close(sock);
    return 0;
}

void responseTreatment(char* response, void *socket) {

  //printf("RESPONSE %s\n", response);
  char* message;
  int sock = *(int*)socket;

  char* method = getRequestType(response);
  char* fileNameRequested = getFileNameRequested(response, method);
  char* contentFileRequested = getContentFileRequested(response);

  printf("CONTENT: %s\n", contentFileRequested);

  if (strcmp(getRequestType(response), "POST") == 0) {

    //if (strcmp(getType(response), "File") == 0) {
      bool isReceived = receive_file(fileNameRequested, contentFileRequested);
      //char* response = receive_file(fileNameRequested, contentFileRequested);
      if (isReceived == true) {
        message = "[SERVER] File received\n";
        write(sock, message, strlen(message));
      }
    //}

  } else if (strcmp(getRequestType(response), "GET") == 0) {
      bool isSend = sendFile(getFileNameRequested(response, method), socket);
      //char* isSend = sendFile(getFileNameRequested(response, method), socket);

  }

  free(method);
  free(contentFileRequested);
  close(sock);
}

bool receive_file(char* file_title, char* file_content) {
  puts("[SERVER] RECEIVE FILE FUNCTION");

  printf("FILE CONTENT %s\n", file_content);

  //char* received_file = "received.txt";
  char* folder = "./received_files/";
  char* result_path = malloc(strlen(folder)+strlen(file_title)+1);
  strcpy(result_path, folder);
  strcat(result_path, file_title);
  //printf("\nFILE PATH %s\n", result_path);

  FILE *rFile = fopen(result_path, "wb");

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

//bool sendFile(char* file_title, int sockfd) {
bool sendFile(char* file_title, void *socket) {

  char* header = "HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n%s";

  int sock = *(int*) socket;
  //char* request = "HTTP/1.1 200 OK\nContent-Type:text/html; charset=UTF-8\nVary: Accept-Encoding\n\n";

  //printf("SEND FILE FUNCTION\n");

  char file_buffer[BUFFSIZE];
  //char* file_name = message;
  //char* folder = "./received_files/";
  //char* result_path = malloc(strlen(folder)+strlen(file_title)+1);
  char* result_path = malloc(strlen(file_title));
  strcat(result_path, file_title);
  //strcpy(result_path, folder);
  //strcat(result_path, file_title);
  //printf("FILE PATH %s\n", result_path);

  printf("[CLIENT] Sending %s to the Client...\n", file_title);

  FILE *file_to_send = fopen(result_path, "rb");
  if(file_to_send == NULL) {
    printf("[ERROR] File %s not found.\n", file_title);
    return false;
  }

  bzero(file_buffer, sizeof(file_buffer));
  int file_size;

  while((file_size = fread(file_buffer, sizeof(char), BUFFSIZE, file_to_send)) > 0) {
      //char* requestToSend = malloc(strlen(request)+strlen(file_buffer) + 1);
      //strcpy(requestToSend, request);
      //strcat(requestToSend, file_buffer);
      printf("MESSAGE %s\n", file_buffer);
      if(send(sock, file_buffer, strlen(file_buffer), 0) < 0) {
          printf("[ERROR] Failed to sent file %s.\n", file_title);
          break;
      }
  }
  fclose(file_to_send);

  printf("[STATUS] Ok File %s from Client was Sent!\n", file_title);

  free(result_path);
  close(sock);
  //free(requestToSend);

  return true;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "tools/HTTPRequestManager.h"

int sendFile(char*, int, char*);
void sendData(char*, int, char*);

typedef enum { false = 0, true = !false } bool;

#define BUFFSIZE 1024 /* default size */
#define PORT 8888 /* port number */

int main(int argc, char *argv[]) {

	/* VARIABLES */
	int sockfd; /* socket */
  int n, m;
	char buffer[BUFFSIZE]; /* Buffer who send file */
	struct sockaddr_in sockaddr_server; /* server addr */
  char message[BUFFSIZE];
  char* postFile = "POST HTTP/1.1 Host: www.google.fr Type: %s FileTitle: %s Content: \"%s\"";
	char* postData = "POST HTTP/1.1 Host: www.google.fr Type: %s Content: \"%s\"";
	char* get = "GET HTTP/1.1 Host: www.google.fr Type: %s Content: \"%s\"";

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
    printf("[CLIENT] Connected to server on port %d\n", PORT);
  }

  while ((n = read(sockfd, buffer, sizeof(buffer)-1)) > 0) {
    buffer[n] = 0;
    fflush(stdin);
    printf("%s",buffer);

    if (scanf("%s", message) != 0) {
      if (sendFile(message, sockfd, postFile) == false){
          printf("THIS IS A DATA\n");
          sendData(message, sockfd, postData);
      }
    }

  }

  if(n < 0) {
      printf("\n [CLIENT] Read error \n");
  }

  printf("[CLIENT] Connection lost.\n");
  close (sockfd);
	return 0;
}

void sendData(char* message, int sockfd, char* postData) {
	char* type = "Data";
	printf("Size post data %lu\n", sizeof(postData));
	printf("Size type %lu\n", sizeof(type));
	printf("Size message %lu\n", sizeof(message));
	int size = sizeof(postData) + sizeof(type) + sizeof(message);
	sprintf(message, postData, "Data", message);
	printf("Message data %s\n", message);
	printf("Message Size %d\n", size);
  if(send(sockfd, message, size , 0) < 0) {
      printf("[ERROR] Failed to sent data.\n");
  }
  message = "";
}

int sendFile(char* message, int sockfd, char* postFile) {

  char file_buffer[BUFFSIZE];
  char* file_name = message;

  printf("[CLIENT] Sending %s to the Server...\n", file_name);

  FILE *file_to_send = fopen(file_name, "r");
  if(file_to_send == NULL) {
    printf("[ERROR] File %s not found.\n", file_name);
    return 0;
  }

  bzero(file_buffer, sizeof(file_buffer));
  int file_size;
  while((file_size = fread(file_buffer, sizeof(char), BUFFSIZE, file_to_send)) > 0) {
      char* file_title;
      sprintf(file_title, "%s", file_name);
      sprintf(message, postFile, "File", file_title, file_buffer);
      printf("MESSAGE %s\n", message);
      if(send(sockfd, message, BUFFSIZE, 0) < 0) {
          printf("[ERROR] Failed to sent file %s.\n", file_name);
          break;
      }
      bzero(file_buffer, sizeof(file_buffer));

  }


  printf("[STATUS] Ok File %s from Client was Sent!\n", file_name);
  message = "";
  return 1;
}

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
void getDataFromServer(char*, int, char*);

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
	char* get = "GET HTTP/1.1 Host: www.google.fr FileTitle: %s";

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

		printf("\n\nSaisir votre méthode de requête:\n 1. Upload\n 2. Download\n\n");
		int choice;
		scanf("%d", &choice);
		switch (choice) {
			case 1:
				printf("Vous allez upload\n");
				if (scanf("%s", message) != 0) {
						sendFile(message, sockfd, postFile) == false ? sendData(message, sockfd, postData) : printf("[ERROR] Send failed\n");
	    	}
				break;

			case 2:
				printf("Vous allez download\n");
				if (scanf("%s", message) != 0) {
					getDataFromServer(message, sockfd, get);
				}
				break;

			default:
				printf("[ERROR] Bad entry !");
		}

  }

  if(n < 0) {
      printf("\n [CLIENT] Read error \n");
  }

  printf("[CLIENT] Connection lost.\n");
  close (sockfd);
	return 0;
}

void getDataFromServer(char* message, int sockfd, char* get) {

	char* messageToSend = malloc(sizeof(char));
	char* type = "Data";

	sprintf(messageToSend, get, message);
	printf("Message to get %s\n", messageToSend);

  if(send(sockfd, messageToSend, strlen(messageToSend) , 0) < 0) {
      printf("[ERROR] Failed to sent data.\n");
  }

  message = "";
	free(messageToSend);

}

void sendData(char* message, int sockfd, char* postData) {

	char* messageToSend = malloc(sizeof(char));
	char* type = "Data";

	sprintf(messageToSend, postData, "Data", message);

  if(send(sockfd, messageToSend, strlen(messageToSend) , 0) < 0) {
      printf("[ERROR] Failed to sent data.\n");
  }

  message = "";
	free(messageToSend);
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
      if(send(sockfd, message, strlen(message), 0) < 0) {
          printf("[ERROR] Failed to sent file %s.\n", file_name);
          break;
      }
      bzero(file_buffer, sizeof(file_buffer));

  }


  printf("[STATUS] Ok File %s from Client was Sent!\n", file_name);
  message = "";
  return 1;
}

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

#define CODE_OK "200 OK"
#define CODE_CREATED "201 CREATED"
#define CODE_MOVED "301 MOVED PERMANENTLY"
#define CODE_FOUND "302 FOUND"
#define CODE_BAD_REQUEST "400 BAD REQUEST"
#define CODE_FORBIDDEN "403 FORBIDDEN"
#define CODE_NOT_FOUND "404 NOT FOUND"
#define CODE_NOT_ALLOWED "405 METHOD NOT ALLOWED"
#define CODE_INTERNAL_ERROR "500 INTERNAL ERROR"
#define CODE_BAD_GETAWAY "502 BAD GATEWAY"
#define CODE_UNAVAILABLE "503 SERVICE UNAVAILABLE"

typedef enum { false = 0, true = !false } bool;

void *connectionHandler(void *);
void responseTreatment(char* , void *);
//bool receiveFile(char*, char*);
char* receiveFile(char* fileTitle, char* fileContent, char* contentType);
//bool sendFile(char* fileTitle, void *socket, char* contentType);
char* sendFile(char* fileTitle, void *socket, char* contentType);
void handlerSignal(int sig);

int   sockfd;
int   newSockfd;
int*  newSocket;

int main(int argc, char *argv[]) {

  /* VARIABLES */
  unsigned int  sockfdSize;
  char          buffer[BUFFSIZE]; /* Buffer who received file */
  //char*         message;

  struct sockaddr_in sockaddrClient; /* client addr */
  //struct sockaddr_in sockaddr_server; /* server addr */

  /* CREATE SOCKET */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
    printf("[ERROR] Could not create socket\n");
    exit(1);
  } else {
    printf("[CLIENT] Socket created sucessfully\n");
  }

  /* INITIALISATION OF SOCKADDR_CLIENT STRUCTURE */
  bzero(&sockaddrClient, sizeof(sockaddrClient));
  sockaddrClient.sin_family = AF_INET; // Protocol
  sockaddrClient.sin_port = htons(PORT); // Port
  sockaddrClient.sin_addr.s_addr = INADDR_ANY; // Address

  /* BIND */
  if( bind(sockfd, (struct sockaddr*)&sockaddrClient, sizeof(struct sockaddr)) == -1 ) {
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

  sockfdSize = sizeof(struct sockaddr_in);
  while((newSockfd = accept(sockfd, (struct sockaddr *)&sockaddrClient, &sockfdSize))) {

    signal(SIGINT, handlerSignal);

    printf("[SERVER] Connection accepted for client %d\n", newSockfd);

    //Thread creation
    pthread_t sniffer_thread;
    newSocket = malloc(1);
    *newSocket = newSockfd;

    //Thread connection handler
    if( pthread_create( &sniffer_thread , NULL ,  connectionHandler , (void*) newSocket) < 0){
        printf("[SERVER] Could not create thread\n");
        return 1;
    }

    printf("[SERVER] Handler assigned to client %d\n", newSockfd);

  }

  if (newSockfd < 0) {
      printf("[SERVER] Failed to accept connection\n");
      return 1;
  }

  close(sockfd);
  close(newSockfd);
}

void handlerSignal(int sig) {

  if (sig == SIGINT) {
    close(sockfd);
    close(newSockfd);
    kill(getpid(), SIGKILL);
  }

}

/*
 * This will handle connection for each client
 * */
void *connectionHandler(void *socket) {

    //Get the socket descriptor
    int   sock = *(int*)socket;
    int   readSize;
    char  clientMessage[BUFFSIZE];

    while( (readSize = read(sock , clientMessage , BUFFSIZE)) > 0 ) {

				if (strlen(clientMessage) == 0) {
            printf("[SERVER] error message is empty\n");
						break ;
				}

        printf("\n[SERVER] Client Response: %s\n|END OF RESPONSE|\n", clientMessage);

        responseTreatment(clientMessage, socket);

    }

    if(readSize == 0) {
        printf("[SERVER] Client disconnected\n");
        fflush(stdout);
    } else if(readSize == -1) {
        printf("[SERVER] Nothing to read\n");
    }

    //Free the socket pointer
    free(socket);
    close(sock);
    return 0;
}

void responseTreatment(char* response, void *socket) {

  int   sock;
  bool  isResponse;
  char* responseServer;
  char* method;
  char* fileNameRequested;
  char* contentFileRequested;
  char* contentType;

  sock = *(int*)socket;
  isResponse = false;

  method = getRequestType(response);
  fileNameRequested = getFileNameRequested(response, method);
  contentFileRequested = getContentFileRequested(response);
  contentType = getContentType(response);

  printf("METHOD: %s\n", method);
  printf("FILE NAME REQUESTED: %s\n", fileNameRequested);
  printf("CONTENT TYPE: %s\n", contentType);

  if (strcmp(method, "POST") == 0) {

      printf("CONTENT: %s\n", contentFileRequested);

      responseServer = receiveFile(fileNameRequested, contentFileRequested, contentType);

      if (responseServer != NULL) {
          isResponse = true;
      }

  } else if (strcmp(method, "GET") == 0) {

      responseServer = sendFile(fileNameRequested, socket, contentType);

      if (responseServer != NULL) {
          isResponse = true;
      }
  }

  if (isResponse) {
    isResponse = false;
    if(send(sock, responseServer, strlen(responseServer), 0) < 0) {
        printf("[ERROR] Failed to sent file %s.\n", fileNameRequested);
    }
  }

  close(sock);
}


char* receiveFile(char* fileTitle, char* fileContent, char* contentType) {

  char*   folder = "./received_files/";
  char*   resultPath = malloc(strlen(folder)+strlen(fileTitle)+1);
  char*   response;

  strcpy(resultPath, folder);
  strcat(resultPath, fileTitle);

  FILE *rFile = fopen(resultPath, "wb");

  if(rFile == NULL) {

    printf("[ERROR] File %s can't be open by the server.\n", fileTitle);
    return responseServerRequest(CODE_NOT_FOUND, "File not found", " ", fileTitle);

  } else {

      int rFile_size = strlen(fileContent);

      int write = fwrite(fileContent, sizeof(char), rFile_size, rFile);

      if(write < rFile_size) {
        printf("[ERROR] File write failed on server.\n");
        return responseServerRequest(CODE_INTERNAL_ERROR, "File can't be write on server", " ", fileTitle);
      }

      if(rFile_size < 0) {
        printf("[ERROR] File not received.\n");
      } else {
        printf("[STATUS] Ok received file %s from client!\n", fileTitle);
      }

      response = responseServerRequest(CODE_OK, " ", " ", fileTitle);
      fclose(rFile);
      return response;
    }

    response = responseServerRequest(CODE_INTERNAL_ERROR, " ", " ", fileTitle);
    return response;
}

char* sendFile(char* fileTitle, void *socket, char* contentType) {

  int    sock = *(int*) socket;
  int    fileSize;
  char   fileBuffer[BUFFSIZE];
  char*  folder = "./received_files/";
  char*  resultPath = malloc(strlen(folder)+strlen(fileTitle)+1);
  char*  response;

  strcpy(resultPath, folder);
  strcat(resultPath, fileTitle);

  if (strcmp(fileTitle, "favicon.ico") == 0) {
    return NULL;
  }

  printf("[CLIENT] Sending %s to the Client...\n", fileTitle);

  FILE *fileToSend = fopen(resultPath, "rb");
  if(fileToSend == NULL) {
    printf("[ERROR] File %s not found.\n", fileTitle);
    return responseServerRequest(CODE_NOT_FOUND, "File not found", " ", fileTitle);
  }

  bzero(fileBuffer, sizeof(fileBuffer));

  fileSize = fread(fileBuffer, sizeof(char), BUFFSIZE, fileToSend);

  response = responseServerRequest(CODE_OK, fileBuffer, getFileContentType(fileTitle), fileTitle);

  printf("####\nMESSAGE AFTER PARSING %s\n####\n", response);

  if (fileSize == 0) {
    return responseServerRequest(CODE_BAD_REQUEST, "File is empty", getFileContentType(fileTitle), fileTitle);
  }

  fclose(fileToSend);

  return response;
}

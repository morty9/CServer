#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "server.h"
#include "tools/HTTPRequestManager.h"
#include "tools/LoggerManager.h"

typedef enum { false = 0, true = !false } bool;

int   sockfd;
int   newSockfd;
int*  newSocket;
struct sockaddr_in sockaddrClient;
pthread_t sniffer_thread;

int socketCreator() {

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    lmCreateSocket(0, 0);
    return 1;
  } else {
    lmCreateSocket(0, 1);
  }

  return 0;

}

void socketInitialization() {

  /* INITIALISATION OF SOCKADDR_CLIENT STRUCTURE */
  bzero(&sockaddrClient, sizeof(sockaddrClient));
  sockaddrClient.sin_family = AF_INET; // Protocol
  sockaddrClient.sin_port = htons(PORT); // Port
  sockaddrClient.sin_addr.s_addr = INADDR_ANY; // Address

}

int socketBinding() {

  if (bind(sockfd, (struct sockaddr*)&sockaddrClient, sizeof(struct sockaddr)) < 0 ) {
    printf("[ERROR] Failed to bind\n");
    return 1;
  } else {
    printf("[SERVER] Bind success, port: %d, address: 127.0.0.1\n",PORT);
  }

  return 0;

}

int socketListening() {

  if(listen(sockfd,3) == -1) {
    printf("[ERROR] Failed to listen on port %d\n", PORT);
    return 1;
  } else {
    printf ("[SERVER] Listening on port %d.\n", PORT);
  }

  return 0;

}

int main(int argc, char *argv[]) {

  /* VARIABLES */
  unsigned int  sockfdSize;
  char          buffer[BUFFSIZE]; /* Buffer who received file */

  socketCreator();
  socketInitialization();
  socketBinding();
  socketListening();

  sockfdSize = sizeof(struct sockaddr_in);
  while((newSockfd = accept(sockfd, (struct sockaddr *)&sockaddrClient, &sockfdSize))) {

    signal(SIGINT, handlerSignal);

    printf("[SERVER] Connection accepted for client %d\n", newSockfd);

    newSocket = malloc(1);
    *newSocket = newSockfd;

    //Thread connection handler
    if( pthread_create( &sniffer_thread , NULL ,  connectionHandler , (void*) newSocket) < 0){
        printf("[SERVER] Could not create thread\n");
        return 1;
    }

    pthread_detach(sniffer_thread);

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
    pthread_exit(&sniffer_thread);
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
        printf("[ERROR] Failed to send file %s.\n", fileNameRequested);
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

      response = responseServerRequest(CODE_OK, "Your file has been sent", " ", fileTitle);
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

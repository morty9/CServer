#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "LoggerManager.h"

char* headerMessage(int src){
    char* message = malloc(sizeof(char) * 64);
    char* tmp = malloc(sizeof(char) * 64);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    sprintf(tmp, "%d\\%d\\%d-%d:%d:%d - ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    strcat(message, tmp);

    if(src == Client){
        strcat(message, "[CLIENT] - ");
    } else {
        strcat(message, "[SERVER] - ");
    }

    free(tmp);
    return message;
}

void messageBody(int src, int success, char* successMsg, char* errorMsg){
    char* message = malloc(sizeof(char) * 256);

    char* header = headerMessage(src);
    strcat(message, header);

    if(success == Success){
        strcat(message, "[ERROR] ");
        strcat(message, errorMsg);
    } else {
        strcat(message, successMsg);
    }

    printf("%s\n", message);

    free(header);
    free(message);
}

void errorMessageBody(int src, char* errorMsg){
    char* message = malloc(sizeof(char) * 256);

    char* header = headerMessage(src);
    strcat(message, header);

    strcat(message, "[ERROR] ");
    strcat(message, errorMsg);

    printf("%s\n", message);

    free(header);
    free(message);
}

void successMessageBody(int src, char* successMsg){
    char* message = malloc(sizeof(char) * 256);

    char* header = headerMessage(src);
    strcat(message, header);
    strcat(message, successMsg);

    printf("%s\n", message);

    free(header);
    free(message);
}

void lmCreateSocket(int src, int success){
    messageBody(src, success, "Socket created sucessfully.", "Could not create socket.");
}

void lmConnectToServer(int src, int success, int port){
    char* tmp = malloc(sizeof(char) * 64);
    sprintf(tmp, "Connected to server on port %d.", port);
    messageBody(src, success, tmp, "Failed to connect to remote.");
    free(tmp);
}

void lmReadError(int src){
    errorMessageBody(src, "Read error.");
}

void lmConnectionLost(int src){
    errorMessageBody(src, "Connection lost.");
}

void lmSendDataFail(int src){
    errorMessageBody(src, "Failed to sent data.");
}

void lmSendingFile(int src, char* filename){
    char* tmp = malloc(sizeof(char) * 256);
    sprintf(tmp, "Sending %s to the server ...", filename);
    successMessageBody(src, tmp);
    free(tmp);
}

void lmFileNotFound(int src, char* filename) {
    char* tmp = malloc(sizeof(char) * 256);
    sprintf(tmp, "File %s not found.", filename);
    errorMessageBody(src, tmp);
    free(tmp);
}

void lmSendFileError(int src, char* filename){
    char* tmp = malloc(sizeof(char) * 256);
    sprintf(tmp, "Failed to sent file %s.", filename);
    errorMessageBody(src, tmp);
    free(tmp);
}

void lmFileSent(int src, char* filename){
    char* tmp = malloc(sizeof(char) * 256);
    sprintf(tmp, "File %s was sent!", filename);
    successMessageBody(src, tmp);
    free(tmp);
}

void lmBind(int src, int success, char* address, int port){
    char* successMsg = malloc(sizeof(char) * 256);
    char* errorMsg = malloc(sizeof(char) * 256);

    sprintf(successMsg, "Bind success - Address: %s - Port: %d.", address, port);
    sprintf(errorMsg, "Failed to bind on %s:%d.", address, port);

    messageBody(src, success, successMsg, errorMsg);

    free(errorMsg);
    free(successMsg);
}

void lmListen(int src, int success, int port){
    char* successMsg = malloc(sizeof(char) * 256);
    char* errorMsg = malloc(sizeof(char) * 256);

    sprintf(successMsg, "Listening on port %d.", port);
    sprintf(errorMsg, "Failed to listen on port %d.", port);

    messageBody(src, success, successMsg, errorMsg);

    free(errorMsg);
    free(successMsg);
}

void lmConnectionSuccess(int src, int socket){
    char* successMsg = malloc(sizeof(char) * 256);
    sprintf(successMsg, "Connection accepted for client %d", socket);
    successMessageBody(src, successMsg);
    free(successMsg);
}

void lmConnectionError(int src){
    char* errorMsg = malloc(sizeof(char) * 256);
    sprintf(errorMsg, "Failed to accept connection");
    errorMessageBody(src, errorMsg);
    free(errorMsg);
}

void lmThreadCreation(int src){
    errorMessageBody(src, "Could not create thread.");
}

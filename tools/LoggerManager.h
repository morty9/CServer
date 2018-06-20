#ifndef LOGGER_MANAGER
#define LOGGER_MANAGER

// Param src to send in each request
enum Source { Client = 1, Server = 0 };

// Param success to send in some requests
enum Result { Success = 1, Error = 0 };

// Socket creation
void lmCreateSocket(int src, int success);

// Connect to server on specified port
void lmConnectToServer(int src, int success, int port);

// Read error
void lmReadError(int src);

// Connection lost
void lmConnectionLost(int src);

// Send data fail
void lmSendDataFail(int src);

// Sending file
void lmSendingFile(int src, char* filename);

// File not found
void lmFileNotFound(int src, char* filename);

// Send file fail
void lmSendFileError(int src, char* filename);

// File sent
void lmFileSent(int src, char* filename);

// Bind
void lmBind(int src, int success, char* address, int port);

// Listen
void lmListen(int src, int success, int port);

// Connection success
void lmConnectionSuccess(int src, int socket);

// Connection error
void lmConnectionError(int src);

// Thread creation
void lmThreadCreation(int src);

#endif

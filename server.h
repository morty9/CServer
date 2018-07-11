#ifndef SERVER
#define SERVER

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

void *connectionHandler(void *);

void responseTreatment(char* , void *);

char* receiveFile(char*, char*, char*);

char* sendFile(char*, void*, char*);

void handlerSignal(int);

#endif

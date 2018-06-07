#ifndef HTTP_REQUEST_MANAGER
#define HTTP_REQUEST_MANAGER


/*int findWord(char* request, char* word)*/

// Get type of the HTTP request
char* getRequestType(char* request);

// Get URL of the request
char* getHost(char* request);

// Get type of content from request
char* getType(char* request);

// Get File Title from request
char* getFileTitle(char* request);

// Get content from request
char* getContent(char* request);


#endif

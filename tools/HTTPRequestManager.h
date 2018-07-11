#ifndef HTTP_REQUEST_MANAGER
#define HTTP_REQUEST_MANAGER


/*int findWord(char* request, char* word)*/

char* getExtensionFile(char*);

char* responseServerRequest(char*, char*, char*, char*);

char* getFileContentType(char*);

// Get name of file requested
char* getFileNameRequested(char*, char*);

char* getContentFileRequested(char*);

char* getBoundary(char*);

char* getContentBetweenBoundary(char*, char*);

size_t getHeaderInfo(char*, char**);

char  *getSubRequest(char*, char*);

char* getContentType(char*);

// Get type of the HTTP request
char* getRequestType(char*);

// Get URL of the request
char* getHost(char*);

// Get type of content from request
char* getType(char*);

// Get File Title from request
char* getFileTitle(char*);

// Get content from request
char* getContent(char*);


#endif

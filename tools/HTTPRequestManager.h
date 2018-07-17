#ifndef HTTP_REQUEST_MANAGER
#define HTTP_REQUEST_MANAGER

/**
 * Get the extension of the selected file
 * @param char* Name of the selected file
 * @return Extension of the selected file
 */
char* getExtensionFile(char*);

/**
 * Format the server request
 * @param char* responseCode Code of the response
 * @param char* bodyContent Content of the body
 * @param char* contentType Content type of the body
 * @param char* fileName Name of the selected file
 * @return Response formatted
 */
char* responseServerRequest(char*, char*, char*, char*);

/**
 * Get the content type of the selected file in the request
 * @param char* Content of the body request
 * @return Extension of the selected file
 */
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




#endif

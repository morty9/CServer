#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "HTTPRequestManager.h"

typedef enum { false = 0, true = !false } bool;

#define CODE_OK = "200 OK"
#define CODE_CREATED = "201 CREATED"
#define CODE_MOVED = "301 MOVED PERMANENTLY"
#define CODE_FOUND = "302 FOUND"
#define CODE_BAD_REQUEST = "400 BAD REQUEST"
#define CODE_FORBIDDEN = "403 FORBIDDEN"
#define CODE_NOT_FOUND = "404 NOT FOUND"
#define CODE_NOT_ALLOWED = "405 METHOD NOT ALLOWED"
#define CODE_INTERNAL_ERROR = "500 INTERNAL ERROR"
#define CODE_BAD_GETAWAY = "502 BAD GATEWAY"
#define CODE_UNAVAILABLE = "503 SERVICE UNAVAILABLE"

struct responseRequest {
  char* protocol;
  char* code;
  char* contentType;
  int contentLenght;
  char* body;
};

/*char* responseRequestType(char* responseCode) {

  char* response;

  switch (responseCode) {
    case CODE_OK:
      response =
    case CODE_CREATED:
    case CODE_MOVED:
    case CODE_FOUND:
    case CODE_BAD_REQUEST:
    case CODE_FORBIDDEN:
    case CODE_NOT_FOUND:
    case CODE_NOT_ALLOWED:
    case CODE_INTERNAL_ERROR:
    case CODE_BAD_GETAWAY:
    case CODE_UNAVAILABLE:
  }

}*/

char* getRequestType(char* request){
    size_t  i;

    i = 0;
    while (request[i] && request[i] == ' ')
      i++;
    if (request[i] == '\0')
      return (NULL);

    return (strndup(request, i));
    //
    // char* tmp = strdup(request);
    //
    // char* info = strtok(tmp, " ");
    // return info;
}

char* getHost(char* request){

    char* tmp = strdup(request);

    char* info = strtok(tmp, " ");
    int match = 0;

    while (info != NULL)
    {
        if(strcmp(info, "Host:") == 0){
            match = 1;
        }

        info = strtok(NULL, " ");

        if(match){
            return info;
        }
    }

    return NULL;
}

char* getType(char* request){

    char* tmp = strdup(request);

    char* info = strtok(tmp, " ");
    int match = 0;

    while (info != NULL)
    {
        if(strcmp(info, "Type:") == 0){
            match = 1;
        }

        info = strtok(NULL, " ");

        if(match){
            return info;
        }
    }

    return NULL;
}


char* getFileNameRequested(char* request, char* method) {

  char* tmp = strdup(request);

  char* info = strtok(tmp, " ");

  while (info != NULL) {

    if (strcmp(method, "GET") == 0) {
      if (strncmp(info, "/", 1) == 0) {
        return strtok(info, "/");
      }
    } else if (strcmp(method, "POST") == 0) {
      if (strncmp(info, "filename", 8) == 0) {
        return strtok(info+9, "\"");;
      }
    }

    info = strtok(NULL, " ");
  }

  return NULL;

}

size_t  getHeaderInfo(char *request, char **boundary)
{
  char    *contentType;
  size_t  i;
  size_t  l;

  contentType = strstr(request, "Content-Type: multipart/");
  if (contentType)
  {
    i = (size_t)contentType - (size_t)request;
    while (request[i] && strncmp(request + i, "boundary=", 9))
      i++;
    if (request[i] == 0)
      return (0);
    i += 9;
    l = 0;
    while (request[i + l] && request[i + l] != '\r')
      l++;
    if ((*boundary = malloc(l + 3)) == NULL)
      return (0);
    strcpy(*boundary, "--");
    strncpy(*boundary + 2, request + i, l);
    (*boundary)[l + 2] = '\0';
  }
  else
  {
    i = 0;
  }
  while (request[i] && strncmp(request + i, "\r\n\r\n", 4))
    i++;
  return ((request[i] ? i + 4 : 0));
}

char  *getSubRequest(char *body, char *boundary)
{
  size_t  i;
  size_t  blen;
  int     count = 2;

  i = 0;
  blen = strlen(boundary);
  while (body[i] && count)
  {
    if (strncmp(body + i, boundary, blen) == 0)
      count--;
    i++;
  }
  if (body[i] == 0)
    return (NULL);
  body[i - 3] = '\0';
  return (body + blen + 2);
}

char* getContentFileRequested(char* request) {
  char    *boundary;
  char    *subRequest;
  size_t  i;

  if (request == NULL)
    return (NULL);
  boundary = NULL;
  i = getHeaderInfo(request, &boundary);
  if (i == 0) {
    return (NULL);
  }

  if (boundary != NULL) {
    subRequest = getSubRequest(request + i, boundary);
    free(boundary);
    return (getContentFileRequested(subRequest));
  } else {
    return (strdup(request + i));
  }

  return NULL;
}

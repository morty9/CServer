#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "HTTPRequestManager.h"

typedef enum { false = 0, true = !false } bool;

char* responseServerRequest(char* responseCode, char* bodyContent, char* contentType, char* FileName) {

  printf("BODY %s\n", bodyContent);

  char* format = "HTTP/1.1 %s\r\nContent-Type: %s; charset=UTF-8\r\n\r\n%s";

  if (responseCode == NULL) {
    return NULL;
  }

  /*if (bodyContent == NULL) {
    bodyContent = malloc(1);
    strcpy(bodyContent, "\0");
  }

  if (contentType == NULL) {
    contentType = malloc(10);
    strcpy(contentType, "text/plain");
  }*/

  char* result = malloc(strlen(format) + strlen(responseCode) + strlen(bodyContent) + strlen(contentType) +1);

  sprintf(result, format, responseCode, contentType, bodyContent);

  return result;
}

char* getExtensionFile(char* fileName) {

  size_t  i;

  if (fileName == NULL) {
    return NULL;
  }

  i = 0;
  while (fileName[i]) {

    if (strncmp(fileName + i, ".", 1) == 0) {
        return strdup(fileName + i + 1);
    }

    i++;
  }

  return NULL;
}

char* getFileContentType(char* fileName) {

  char*   type;
  char*   extension;

  if (fileName == NULL) {
    return NULL;
  }

  extension = getExtensionFile(fileName);

  printf("EXTENSION %s\n", extension);

  if (strcmp(extension, "txt") == 0 || strcmp(extension, "rtf") == 0) {
    type = "text/plain";
  } else if (strcmp(extension, "json") == 0) {
    type = "application/json";
  } else if (strcmp(extension, "html") == 0) {
    type = "text/html";
  }

  printf("TYPE %s\n", type);

  return type;
}

char* getRequestType(char* request) {
    size_t  i;
    bool    found = false;

    i = 0;
    while (request[i] && request[i] != ' ') {
      i++;
    }

    if (request[i] == '\0') {
      return NULL;
    }

    return strndup(request, i);
}

char* getContentType(char* body) {

  size_t  i = -2;
  size_t  size = 0;

  if (body == NULL) {
    return NULL;
  }

  while (body[size]) {

    if (strncmp(body + size, "Content-Type: ", 14) == 0) {
      i++;
      size += 14;
    }

    if (i == 0) {
      return strtok(body + size, "\r\n");
    }

    size++;
  }

  return NULL;
}

char* getFileNameRequested(char* request, char* method) {

  char*   tmp;
  char*   info;

  tmp = strdup(request);
  info = strtok(tmp, " ");

  if (request == NULL || method == NULL) {
    return NULL;
  }

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

size_t  getHeaderInfo(char* request, char** boundary) {

  char*   contentType;
  size_t  i;
  size_t  l;

  if (request == NULL || boundary == NULL) {
    return 0;
  }

  contentType = strstr(request, "Content-Type: multipart/");

  if (contentType) {

    i = (size_t)contentType - (size_t)request;

    while (request[i] && strncmp(request + i, "boundary=", 9)) {
      i++;
    }

    if (request[i] == 0) {
      return 0;
    }

    i += 9;
    l = 0;

    while (request[i + l] && request[i + l] != '\r') {
      l++;
    }

    if ((*boundary = malloc(l + 3)) == NULL) {
      return 0;
    }

    strcpy(*boundary, "--");
    strncpy(*boundary + 2, request + i, l);
    (*boundary)[l + 2] = '\0';

  } else {
    i = 0;
  }

  while (request[i] && strncmp(request + i, "\r\n\r\n", 4)) {
    i++;
  }

  return (request[i] ? i + 4 : 0);
}

char*  getSubRequest(char* body, char* boundary) {
  size_t  i;
  size_t  blen;
  int     count = 2;

  i = 0;
  blen = strlen(boundary);

  while (body[i] && count) {

    if (strncmp(body + i, boundary, blen) == 0) {
      count--;
    }
    i++;
  }

  if (body[i] == 0) {
    return NULL;
  }

  body[i - 3] = '\0';
  return (body + blen + 2);
}

char* getContentFileRequested(char* request) {

  char*    boundary;
  char*    subRequest;
  size_t  i;

  if (request == NULL) {
    return NULL;
  }

  boundary = NULL;
  i = getHeaderInfo(request, &boundary);

  if (i == 0) {
    return NULL;
  }

  if (boundary != NULL) {
    subRequest = getSubRequest(request + i, boundary);
    free(boundary);
    return getContentFileRequested(subRequest);
  } else {
    return strdup(request + i);
  }

  return NULL;
}

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "HTTPRequestManager.h"

typedef enum { false = 0, true = !false } bool;

/*int findWord(char* request, char* word){

    int requestLength = strlen(request);
    int wordLength = strlen(word);

    for(int i = 0; i < requestLength; i++){
        int j;

        for(j = 0; j < wordLength; j++){
            if(request[i+j] != word[j]){
                break;
            }

            if(j == wordLength - 1){
                return i;
            }
        }
    }

    return -1;
}*/


char* getRequestType(char* request){

    char* tmp = strdup(request);

    char* info = strtok(tmp, " ");
    return info;
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
    if ((*boundary = strndup(request + i, l)) == NULL)
      return (0);
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

  printf("BODY %s\n", body);
  printf("BOUNDARY %s\n", boundary);

  i = 0;
  blen = strlen(boundary);
  while (body[i] && count)
  {
    if (strncmp(body + i, boundary, blen) == 0)
      count--;
    i++;
  }
  return (strndup(body + blen + 2, i - (blen + 2 + 2))); // mais si c'est facile
}

char* getContentFileRequested(char* request) {
  char    *boundary;
  size_t  i;

  if(request == NULL)
    return (NULL);

  i = getHeaderInfo(request, &boundary);
  printf("%lu\n", i);
  if (i == 0) {
    return (NULL);
  }

  if (boundary != NULL) {
    return (getContentFileRequested(getSubRequest(request + i, boundary)));
  } else {
    return (strdup(request + i));
  }

  return NULL;
}

//-------------------------------------------------------------------------

// char* getBoundary(char* request) {
//
//   char* tmp = strdup(request);
//
//   char* info = strtok(tmp, ";");
//
//
//
//
//
//
//
//
//   while (info != NULL) {
//
//     if (strncmp(info, " boundary=", 10) == 0) {
//       return strtok(info+10, "\r");
//     }
//
//     info = strtok(NULL, ";");
//
//   }
//
//   return NULL;
// }
//
// char* getBody (char* request) {
//   size_t i;
//
//   i = 0;
//   while (request[i] && strncmp(request + i, "\r\n\r\n", 4)) {
//     i++;
//   }
//   return (request + i);//
// }
//
// char* getContentBetweenBoundary(char* request, char* boundary) {
//
//   //printf("GET CONTENT BETWEEN : ---------|||%s|||----------\n", request);
//   //printf("BOUNDARY : ---------|||%s|||----------\n", boundary);
//
//   int start = 0;
//   int len, bLen;
//   // int isBoundary= 0;
//
//   bLen = strlen(boundary);
//   while (request[start] && strncmp(request + start, boundary, bLen)) {
//     start++;
//   }
//   if (request[start] == 0)
//     return NULL;
//   start += bLen;
//   len = 0;
//   while (request[start + len] && strncmp(request + start + len, boundary, bLen)) {
//     len++;
//   }
//   if (request[start + len] == 0)
//     return NULL;
//   return (strndup(request + start, len));
//   // //--------------
//   // while (request[start]) {
//   //
//   //   //printf("\n@@@@STRLEN BOUNDARY\n %lu\n@@@@\n", strlen(boundary));
//   //   if (strncmp(request+start, boundary, strlen(boundary)) == 0) {
//   //     isBoundary++;
//   //     start+=strlen(boundary);
//   //     printf("\n********\nREQUEST START BOUNDARY\n %s \n********\n", request+start);
//   //   }
//   //
//   //   start++;
//   // }
//   //
//   // while (request[start + len]) {
//   //   if (strncmp(request+start+len, boundary, strlen(boundary)) == 0) {
//   //     printf("\n||||||\n %s \n||||||\n", request+start);
//   //     return strndup(request+start, len-2);
//   //   }
//   //   len++;
//   // }
//   //
//   //
//   // return NULL;
// }
//
// char* getContentFileRequested(char* request) {
//
//   int start = 0;
//   int isBody = -2;
//   char* boundary;
//   char* resultContent;
//
//   while (request[start] && isBody <= 0)
//   {
//     if (strncmp(request+start, "Content-Type: ", 14) == 0) {
//
//       isBody++;
//       start += 13;
//
//       if (strncmp(request+start, " multipart/", 11) == 0) {
//         boundary = getBoundary(request+start);
//         //printf("\n\n'''BOUNDARY %s'''\n\n", boundary);
//         start+=strlen(boundary);
//         resultContent = getContentBetweenBoundary(request+start, boundary);
//         printf("RESULT CONTENT#### %s####\n", resultContent);
//       }
//
//     }
//
//     if (isBody == 0 && strncmp(request+start, "\r\n\r\n", 4) == 0) {
//       isBody++;
//       start += 3;
//     }
//
//     //if ()
//
//     start++;
//   }
//
//   if (request[start] == 0) {
//     return  NULL;
//   }
//
//   return NULL;
//   //return strdup(request+start);
//   //return strtok(request+start, "\r\n");
//
// }

// //antiquité
// char* getContent(char* request){
//
//     char* tmp = strdup(request);
//
//     char* info = strtok(tmp, " ");
//     int match = 0;
//     int i = strlen(info) + 1;
//
//     char* type;
//     char* cont;
//
//     type = malloc(sizeof(char) * 500);
//
//     while (info != NULL)
//     {
//
//         if(strcmp(info, "Content:") == 0){
//             info = strtok(NULL, " ");
//             i += strlen(info) + 3;
//             cont = strdup(&request[i]);
//             cont[strlen(cont) - 1] = '\0';
//             return cont;
//         }
//
//         if(strcmp(info, "Type:") == 0){
//             info = strtok(NULL, " ");
//             strcpy(type, info);
//         }
//
//         info = strtok(NULL, " ");
//         i += strlen(info) + 1;
//
//     }
//
//     return NULL;
// }


/*int main(int argc, char *argv[]) {
    char* requestContent = strdup("GET HTTP/1.1 Host: www.google.fr Type: Data Content: \"Je suis beaucoup trop chaud\"");
    char* requestFile = strdup("POST HTTP/1.1 Host: www.google.fr Type: File FileTitle: file.txt Content: \"Je suis beaucoup trop chaud\"");

    printf("%s\n", getRequestType(requestFile));
    printf("Host: %s\n", getHost(requestFile));
    printf("Type: %s\n", getType(requestFile));
    printf("FileTitle: %s\n", getFileTitle(requestFile));
    printf("Content: %s\n", getContent(requestFile));

    char* requestContentV2 = "GET /?Type=Data&Content=azert HTTP/1.1";
    char* requestFileV2 = "GET /?Type=File&FileTitle=test.txt&Content=zertyuio HTTP/1.1";


}*/

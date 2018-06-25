#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "HTTPRequestManager.h"

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


char* getFileTitle(char* request){
    char* tmp = strdup(request);

    char* info = strtok(tmp, " ");

    while(info != NULL){

        if(strcmp(info, "Type:") == 0){
            info = strtok(NULL, " ");

            if(strcmp(info, "File") != 0){
                return NULL;
            }
        } else if(strcmp(info, "FileTitle:") == 0){
            info = strtok(NULL, " ");
            return info;
        } else {
            info = strtok(NULL, " ");
        }
    }

    return NULL;
}

/*char* getContent(char* request){

    char* tmp = malloc(sizeof(char) * strlen(request));
    strcpy(tmp, request);

    char* info = strtok(tmp, " ");
    int match = 0;

    char* type;

    while (info != NULL)
    {
        if(strcmp(info, "Type:") == 0){
            info = strtok(NULL, " ");
            strcpy(type, info);
        }

        if(strcmp(info, "Content:") == 0){
            match = 1;
        }

        info = strtok(NULL, " ");

        if(match){
            if(strcmp(type, "Data") == 0){
                memmove(info, info+1, strlen(info));

                info = strcat(info, " ");
                info = strcat(info, strtok(NULL, "\0"));
                info[strlen(info) - 1] = '\0';

                return info;
            } else if(strcmp(type, "File") == 0) {
                memmove(info, info+1, strlen(info));

                info = strcat(info, " ");
                info = strcat(info, strtok(NULL, "\0"));
                info[strlen(info) - 1] = '\0';

                return info;
            }
        }
    }

    return NULL;
}*/


char* getContent(char* request){

    char* tmp = strdup(request);

    char* info = strtok(tmp, " ");
    int match = 0;
    int i = strlen(info) + 1;

    char* type;
    char* cont;

    type = malloc(sizeof(char) * 500);

    while (info != NULL)
    {

        if(strcmp(info, "Content:") == 0){
            info = strtok(NULL, " ");
            i += strlen(info) + 3;
            cont = strdup(&request[i]);
            cont[strlen(cont) - 1] = '\0';
            return cont;
        }

        if(strcmp(info, "Type:") == 0){
            info = strtok(NULL, " ");
            strcpy(type, info);
        }

        info = strtok(NULL, " ");
        i += strlen(info) + 1;

    }

    return NULL;
}


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

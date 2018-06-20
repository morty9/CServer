#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/uio.h>
#define NTHREADS 100

// Socket create
int sock_create( )
{
  /* CREATE SOCKET */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
    printf("[ERROR] Could not create socket\n");
    exit(1);
  } else {
    printf("[CLIENT] Socket created sucessfully\n");
  }

    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 2100);

}
// Bind
int sock_bind()
{
    int b= bind(sockfd,(struct sockaddr *)&server , sizeof(server));
    if (b <0)
    {
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind");

}
// Listen
int sock_listen()
{
   listen(sockfd , 10);
}
//Connection accept
int sock_accept()
{
    int s = sizeof(struct sockaddr_in);
    d1= accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&s);

    if (d1 < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
}

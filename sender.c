#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <time.h>
#define SIZE 1024
#define FILESIZE 1048576

void send_file(FILE *fp, int sockfd)
{
  char data[SIZE] = {0};

  while (fgets(data, SIZE, fp) != NULL)
  {
    if (send(sockfd, data, SIZE, 0) == -1)
    {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}

int main()
{
  char *ip = "127.0.0.1"; //"this computer's" ip
  int port = 8080;
  int pair;
  int socket_file;
  struct sockaddr_in server_addr;
  FILE *file_pointer;
  char *filename = "1gb.txt";

  char cc_algo[256];
  socklen_t length = sizeof(cc_algo);
  for (int k = 0; k < 5; k++)
  {
    //create a socket for recieving file, and its IP adress family is ipv4, and the socket is for file streaming.
    socket_file = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file < 0)
    {
      perror("[-]Error in socket creation");
      exit(1);
    }
    printf("[+]Server's socket is created w/o errors.\n");
    //intialize server adress obj
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    pair = connect(socket_file, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (pair < 0)
    {
      perror("[-]Error in socket connection");
      exit(1);
    }
    printf("[+]Socket connection was succsessfull\n");
    file_pointer = fopen(filename, "r");

    if (file_pointer == NULL)
    {
      perror("[-]Error file not found");
      exit(1);
    }
    //default cc algorithm is cubic#include <stdio.h>
    getsockopt(socket_file, IPPROTO_TCP, TCP_CONGESTION, cc_algo, &length);
    send_file(file_pointer, socket_file);
    printf("[+]File#%d  data sent successfully with [%s]cc.\n", k + 1, cc_algo);
  }
 
  for (int k = 0; k < 5; k++)
  {
    //create a socket for recieving file, and its IP adress family is ipv4, and the socket is for file streaming.
    socket_file = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file < 0)
    {
      perror("[-]Error in socket creation");
      exit(1);
    }
    printf("[+]Server's socket is created w/o errors.\n");
    //intialize server adress obj
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    pair = connect(socket_file, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (pair < 0)
    {
      perror("[-]Error in socket connection");
      exit(1);
    }
    printf("[+]Socket connection was succsessfull\n");
    //change cc algo
    strcpy(cc_algo, "reno");
    setsockopt(socket_file, IPPROTO_TCP, TCP_CONGESTION, cc_algo, length);
    file_pointer = fopen(filename, "r");

    if (file_pointer == NULL)
    {
      perror("[-]Error file not found");
      exit(1);
    }
    //default cc algorithm is cubic#include <stdio.h>
    getsockopt(socket_file, IPPROTO_TCP, TCP_CONGESTION, cc_algo, &length);
    send_file(file_pointer, socket_file);
    printf("[+]File#%d  data sent successfully with [%s]cc.\n", k + 1, cc_algo);
  }

  close(socket_file);
  printf("[ ]Disconneced from server\n");
  return 0;
}
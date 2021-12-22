#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>

#define SIZE 1024
#define FILESIZE 1048576
double write_file(int server_socket, FILE *filepointer,int index)
{
    int read_from_socket;
    char data_buffer[SIZE];

    char cc_algo[256];  
    socklen_t length = sizeof(cc_algo);
    getsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, cc_algo, &length);

    int buffers_needed = FILESIZE / SIZE;
    int data_left = FILESIZE % SIZE;
    double total_time = 0;

    struct timespec current_start, current_finish;
    clock_gettime(CLOCK_MONOTONIC, &current_start);
    for (int buffer_index = 0; buffer_index < buffers_needed; buffer_index++)
    {
        //rcv data from socket to buffer
        recv(server_socket, data_buffer, SIZE, 0);
        //write data from buffer to file
        fwrite(data_buffer, SIZE, 1, filepointer);
        //empty data buffer;
        bzero(data_buffer, SIZE);
    }
    // in case our buffer does not fully cover the file size, we want to load the remaining data (instead of loading SIZE amount)
    if (data_left > 0)
    {
        recv(server_socket, data_buffer, data_left, 0);
        fwrite(data_buffer, data_left, 1, filepointer);
        bzero(data_buffer, SIZE);
    }

    clock_gettime(CLOCK_MONOTONIC, &current_finish);
    uint64_t actual_start = current_start.tv_sec * 1000000000 + current_start.tv_nsec;
    uint64_t actual_finish = current_finish.tv_sec * 1000000000 + current_finish.tv_nsec;
    double current_time = (actual_finish - actual_start) * 0.000000001;
    printf("[+]File#%d recieved in %f seconds. CC algo = [%s]\n",index+1, current_time,cc_algo);
    return current_time;
}

int main()
{
    FILE *file_pointer;
    char filename[] = "file_recieved.txt";
    file_pointer = fopen(filename, "w");
    if (file_pointer == NULL)
    {
        perror("[-]Error in file creation");
        exit(1);
    }

    char *ip = "127.0.0.1"; //"this computer's" ip
    int port = 8080;
    int pair;
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    memset(&server_addr, 0, sizeof(server_addr));
    char buffer[SIZE];
    //create a sock stream type of socket descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("[-]Error in socket creation");
        exit(1);
    }
    printf("[+]Server's socket is created w/o errors.\n");
    //set the server's address for the default
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        perror("[-]Error in socket binding");
        exit(1);
    }
    printf("[+]Socket binding was done w/o errors\n");
    char cc_algo[256];  
    socklen_t length = sizeof(cc_algo);
    getsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, cc_algo, &length);
    
    double total_time = 0;
    for (int i = 0; i < 5; i++)
    {
        pair = listen(server_socket, 10);
        if (pair < 0)
        {
            perror("[-]Error in socket listening");
            exit(1);
        }
        printf("[+]Currently listening..\n");
        int file_count = 0;
        addr_size = sizeof(new_addr);
        new_socket = accept(server_socket, (struct sockaddr *)&new_addr, &addr_size);
        if (new_socket < 0)
        {
            perror("[-]Socket listen failed\n");
            exit(1);
        }
        total_time += write_file(new_socket, file_pointer,i);
    }
    printf("[+]~~~~~~Avg of [%s] CC algo is %f~~~~~~\n",cc_algo,total_time);

    //accept 5 files again with different algo

    total_time = 0;
    for (int i = 0; i < 5; i++)
    {
        pair = listen(server_socket, 10);
        if (pair < 0)
        {
            perror("[-]Error in socket listening");
            exit(1);
        }
        printf("[+]Currently listening..\n");
        int file_count = 0;
        addr_size = sizeof(new_addr);
        new_socket = accept(server_socket, (struct sockaddr *)&new_addr, &addr_size);
        if (new_socket < 0)
        {
            perror("[-]Socket listen failed\n");
            exit(1);
        }
        //change default cc algo to reno
        strcpy(cc_algo, "reno");
        setsockopt(new_socket, IPPROTO_TCP, TCP_CONGESTION, cc_algo, length);

        total_time += write_file(new_socket, file_pointer,i);
    }
    printf("[+]~~~~~~Avg of [reno] CC algo is %f~~~~~~\n",total_time);


    close(server_socket);
    close(new_socket);
    printf("[ ] Server disconnected.\n");
    return 0;
}

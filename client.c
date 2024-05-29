#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int create_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void connect_to_server(int sockfd, const char *server_ip, int server_port)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");
}

void send_command(int sockfd, char *command)
{
    if (send(sockfd, command, strlen(command), 0) < 0)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

void receive_response(int sockfd)
{
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_received] = '\0';
    printf("Server response: %s\n", buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    int sockfd = create_socket();
    connect_to_server(sockfd, argv[1], 1234);
    send_command(sockfd, "hello");
    receive_response(sockfd);
    close(sockfd);
    return 0;
}
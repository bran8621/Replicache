#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>

#define PORT 1234
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

int init_server()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    return sock;
}

void write_client(int clientfd, char *message)
{
    if (write(clientfd, message, strlen(message)) < 0)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("Sent: %s\n", message);
}

void read_client(int clientfd, struct pollfd *fds, int index)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(clientfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0)
    {
        perror("recv");
        close(clientfd);
        fds[index].fd = -1; // Marking the file descriptor as unused
        return;
    }
    else if (bytes_received == 0)
    {
        printf("Client disconnected\n");
        close(clientfd);
        fds[index].fd = -1; // Marking the file descriptor as unused
        return;
    }

    printf("Received: %s\n", buffer);

    write_client(clientfd, "OK\n");

    // Clear revents field after handling POLLIN event
    fds[index].revents = 0;
}

int main()
{
    int serverfd = init_server();

    struct pollfd fds[MAX_CLIENTS];
    int client_count = 0;

    // Initialize pollfd array
    fds[0].fd = serverfd;
    fds[0].events = POLLIN;

    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }

    while (1)
    {
        int poll_count = poll(fds, MAX_CLIENTS, -1);
        if (poll_count < 0)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN)
        {
            // Accept new connection
            int new_socket = accept(serverfd, NULL, NULL);
            if (new_socket < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection, socket fd is %d\n", new_socket);

            // Add new socket to pollfd array
            int added = 0;
            for (int i = 1; i < MAX_CLIENTS; i++)
            {
                if (fds[i].fd == -1)
                {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN; // Set events to monitor for new socket
                    added = 1;
                    break;
                }
            }

            if (!added)
            {
                printf("Max clients reached. Closing new connection.\n");
                close(new_socket);
            }
        }

        for (int i = 1; i < MAX_CLIENTS; i++)
        {
            if (fds[i].fd != -1 && (fds[i].revents & POLLIN))
            {
                read_client(fds[i].fd, fds, i);
            }
        }
    }

    close(serverfd);
    return 0;
}

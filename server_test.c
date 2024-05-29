#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

#define BUFFER_SIZE 1024
#define PORT 1234
#define LOCALHOST "127.0.0.1"
#define NUM_CLIENTS 5

void start_server()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        execl("./server", "./server", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    // Give the server some time to start up
    sleep(1);
}

void test_client_interaction(int client_id)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, LOCALHOST, &server_addr.sin_addr);

    int connect_status = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    assert(connect_status >= 0);

    char *commands[] = {
        "hello\n",
        "world\n",
        "test\n",
        "q\n"};

    for (int i = 0; i < 4; i++)
    {
        send(sockfd, commands[i], strlen(commands[i]), 0);
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        assert(bytes_received >= 0);
        buffer[bytes_received] = '\0';
        printf("Client %d - Server response: %s\n", client_id, buffer);
    }

    close(sockfd);
}

void run_clients()
{
    pid_t pids[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0)
        {
            test_client_interaction(i + 1);
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}

int main()
{
    start_server();

    run_clients();

    // Wait for a moment to ensure all clients have finished
    sleep(2);

    // You might want to have a mechanism to stop the server here if needed

    return 0;
}

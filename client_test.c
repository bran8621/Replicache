#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

#define BUFFER_SIZE 1024
#define PORT 1234
#define LOCALHOST "127.0.0.1"

void test_client_interaction()
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
        printf("Server response: %s\n", buffer);
    }

    close(sockfd);
}

int main()
{
    test_client_interaction();
    return 0;
}

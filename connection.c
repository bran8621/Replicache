#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"

typedef struct
{
    int fd;
    char read_buffer[BUFFER_SIZE];
    char write_buffer[BUFFER_SIZE];

    size_t read_buffer_size;
    size_t write_buffer_size;

    size_t read_offset;
    size_t write_offset;
} Connection;

Connection *init_connection(int fd)
{
    Connection *connection = malloc(sizeof(Connection));

    connection->fd = fd;
    connection->read_buffer_size = 0;
    connection->write_buffer_size = 0;
    connection->read_offset = 0;
    connection->write_offset = 0;
    return connection;
}

void set_fd_nb(int fd)
{
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    if (errno != 0)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    return;
}
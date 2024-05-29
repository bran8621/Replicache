#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "1234"
#define LOCALHOST "127.0.0.1"
void prompt(char *port)
{

    while (1)
    {
        char *buf = malloc(1024);
        printf("replicache %s: %s > ", LOCALHOST, port);
        fgets(buf, 1024, stdin);
        if (buf[0] == 'q')
            break;
        printf("\n %s\n", buf);
        free(buf);
    }
}

int main(int argc, char *argv[])
{
    prompt(PORT);
    return 0;
}
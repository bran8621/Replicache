#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    const char *command;
    union
    {
        int num_args;
        struct
        {
            int num_args;
            int *args;
        };
    } Args;
} CommandInfo;

bool is_command(const char *command)
{
    for (int i = 0; i < sizeof(commandDictionary) / sizeof(commandDictionary[0]); i++)
    {
        if (!strcmp(command, commandDictionary[i].command))
        {
            return true;
        }
    }
    return false;
}

CommandInfo commandDictionary[] = {
    // key-value commands
    {"SET", 2},
    {"GET", 1},
    {"DEL", 1},
    {"EXISTS", 1},
    {"TYPE", 1},
    {"EXPIRE", 2},
    {"PERSIST", 1},

    // string commands
    {"INCR", 1},
    {"DECR", 1},
    {"INCRBY", 2},
    {"DECRBY", 2},

    // hash commands
    {"HSET", 3},
    {"HGET", 2},
    {"HGETALL", 1},
    {"HDEL", 2},
    {"HEXISTS", 2},
    {"HKEYS", 1},
    {"HVALS", 1},
    {"HLEN", 1},

    // list operations
    {"LPUSH", 2},
    {"RPUSH", 2},
    {"LPOP", 1},
    {"RPOP", 1},
    {"LLEN", 1},
    {"LRANGE", 3},
    {"LINDEX", 2},
    {"LSET", 3},
    {"LREM", 3},
    {"LPOS", {3, 4}},
};

char **tokenize_char_array(const char *str, const char *delim, int *num_tokens)
{
    char *str_copy = strdup(str);
    if (!str_copy)
    {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    char **tokens = malloc(sizeof(char *));

    if (!tokens)
    {
        perror("malloc");
        free(str_copy);
        exit(EXIT_FAILURE);
    }

    char *token = strtok(str_copy, delim);

    while (token != NULL)
    {
        tokens = realloc(tokens, (count + 1) * sizeof(char *));
        if (!tokens)
        {
            perror("realloc");
            free(str_copy);
            free(tokens);
            exit(EXIT_FAILURE);
        }

        tokens[count] = strdup(token);
        if (!tokens[count])
        {
            perror("strdup");
            free(str_copy);
            for (int i = 0; i < count; i++)
            {
                free(tokens[i]);
            }
            free(tokens);
            exit(EXIT_FAILURE);
        }
        count++;
        token = strtok(NULL, delim);
    }
    free(str_copy);

    *num_tokens = count;
    return tokens;
}

void free_tokens(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
    tokens = NULL;

    return;
}

int main(int argc, char *argv[])
{
    char buff[1024];
    int num_tokens = 0;
    printf("Enter string to be tokenized: ");
    fgets(buff, sizeof(buff), stdin);
    char **tokens = tokenize_char_array(buff, " ", &num_tokens);

    for (int i = 0; i < num_tokens; i++)
    {
        printf("Token %d: %s\n", i, tokens[i]);
    }

    free_tokens(tokens);

    return 0;
}
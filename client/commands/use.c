/*
** Project  -  my_teams
** Date     -  April 15th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include "commands.h"

void define_context_level(char *line)
{
    char context_level[32];

    if (sscanf(line + 4, "%31s", context_level) != 1)
        return;
    if (strcmp(context_level, "GLOBAL") == 0)
        current_context.context_level = GLOBAL;
    else if (strcmp(context_level, "TEAM") == 0)
        current_context.context_level = TEAM;
    else if (strcmp(context_level, "CHANNEL") == 0)
        current_context.context_level = CHANNEL;
    else if (strcmp(context_level, "THREAD") == 0)
        current_context.context_level = THREAD;

    printf("%s\n", context_level);
}

void send_server_context(char **args, int client_socket)
{
    char cmd[1024];

    if (args[1] == NULL) {
        snprintf(cmd, sizeof(cmd), "USE\r\n");
        write(client_socket, cmd, strlen(cmd));
    } else if (args[2] == NULL) {
        snprintf(cmd, sizeof(cmd), "USE \"%s\"\r\n", args[1]);
        write(client_socket, cmd, strlen(cmd));
    } else if (args[3] == NULL) {
        snprintf(cmd, sizeof(cmd), "USE \"%s\" \"%s\"\r\n", args[1], args[2]);
        write(client_socket, cmd, strlen(cmd));
    } else {
        snprintf(cmd, sizeof(cmd), "USE \"%s\" \"%s\" \"%s\"\r\n", args[1], args[2], args[3]);
        write(client_socket, cmd, strlen(cmd));
    }
}

int cmd_use(char **args, int client_socket)
{
    send_server_context(args, client_socket);

    // Wait respons
    char response[1024];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    // If server didnt respond logout user
    if (res_read <= 0)
        return cmd_logout(args, client_socket);
    response[res_read] = '\0';

    char *line = strtok(response, "\r\n");
    while (line != NULL) {
        if (strstr(line, "USE") != NULL) {
            define_context_level(line);
            break;
        } else if (strncmp(line, "404", 3) == 0) {
            printf("-> ERROR: Cannot define context.\n");
            break;
        }
        line = strtok(NULL, "\r\n");
    }

    return 1;
}
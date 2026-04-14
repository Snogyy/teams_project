/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../client.h"

void cmd_user(char **args, int client_socket)
{
    if (args[1] == NULL || args[2] != NULL) {
        printf("Usage: /user \"user_uuid\"\n");
        return;
    }

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "USER \"%s\"\r\n", args[1]);
    write(client_socket, cmd, strlen(cmd));

    char response[4096];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    if (res_read <= 0) {
        printf("Server disconnected.\n");
        return;
    }
    response[res_read] = '\0';

    char *line = strtok(response, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "USER ", 5) == 0) {
            char uuid[UUID_LENGTH], name[MAX_NAME_LENGTH];
            int status;
            if (sscanf(line + 5, "%36s %31s %d", uuid, name, &status) == 3)
                client_print_user(uuid, name, status);
        } else if (strncmp(line, "404", 3) == 0) {
            client_error_unknown_user(args[1]);
        } else if (strncmp(line, "401", 3) == 0) {
            client_error_unauthorized();
        }
        line = strtok(NULL, "\r\n");
    }
}
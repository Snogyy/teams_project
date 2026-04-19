/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../client.h"
#include "commands.h"

int cmd_send(char **args, int client_socket)
{
    if (args[1] == NULL || args[2] == NULL || args[3] != NULL) {
        printf("Usage: /send \"user_uuid\" \"message_body\"\n");
        return 1;
    }
    if (strlen(args[2]) > MAX_BODY_LENGTH - 1) {
        printf("Message is too long\n");
        return 1;
    }

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "SEND \"%s\" \"%s\"\r\n", args[1], args[2]);
    write(client_socket, cmd, strlen(cmd));

    char response[4096];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    if (res_read <= 0)
        return cmd_logout(args, client_socket);
    response[res_read] = '\0';

    char *line = strtok(response, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "404", 3) == 0)
            client_error_unknown_user(args[1]);
        else if (strncmp(line, "401", 3) == 0)
            client_error_unauthorized();
        else if (strncmp(line, "413", 3) == 0)
            printf("Message too long\n");
        line = strtok(NULL, "\r\n");
    }
    return 1;
}
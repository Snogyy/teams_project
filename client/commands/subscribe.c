/*
** Project  -  my_teams
** Date     -  April 14st 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include <uuid/uuid.h>

int cmd_subscribe(char **args, int client_socket)
{
    if (current_user.is_logged == false) {
        printf("-> User not logged.\n");
        return 1;
    }
    if (args[1] == NULL || args[2] != NULL) {
        printf("-> Usage: /subscribe [\"team_uuid\"]\n");
        return 1;
    }

    // Send command to server with double quotes around the username
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "SUB \"%s\"\r\n", args[1]);
    write(client_socket, cmd, strlen(cmd));
    char response[1024];
    int res_read = read(client_socket, response, sizeof(response) - 1);

    if (res_read <= 0)
        return 1;
    response[res_read] = '\0';
    char *line = strtok(response, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "240", 3) == 0) {
            char user_uuid[UUID_LENGTH] = {0};
            char team_uuid[UUID_LENGTH] = {0};

            if (sscanf(line + 4, "%36s %36s", user_uuid, team_uuid) == 2)
                client_print_subscribed(user_uuid, team_uuid);
            return 1;
        }
        if (strncmp(line, "401", 3) == 0) {
            client_error_unauthorized();
            return 1;
        }
        if (strncmp(line, "440", 3) == 0) {
            client_error_unknown_team(args[1]);
            return 1;
        }
        line = strtok(NULL, "\r\n");
    }
    printf("-> Subscribe failed: %s\n", response);
    return 1;
}
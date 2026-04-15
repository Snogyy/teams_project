/*
** Project  -  my_teams
** Date     -  April 14st 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include <uuid/uuid.h>

int cmd_unsubscribe(char **args, int client_socket)
{
    if (current_user.is_logged == false) {
        printf("-> Please login first.\n");
        return 1;
    }
    if (args[1] == NULL || args[2] != NULL) {
        printf("-> Usage: /unsubscribe [\"team_uuid\"]\n");
        return 1;
    }

    // Send command to server with double quotes around the username
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "UNSU \"%s\"\r\n", args[1]);
    write(client_socket, cmd, strlen(cmd));

    char response[1024];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    if (res_read <= 0)
        return 1;
    response[res_read] = '\0';

    if (strncmp(response, "242", 3) == 0) {
        // Success
        char *user_uuid = strtok(response + 4, " ");
        char *team_uuid = strtok(NULL, " ");
        if (user_uuid && team_uuid) {
            client_print_unsubscribed(user_uuid, team_uuid);
        }
    } else {
        printf("-> Unsubscription failed: %s\n", response);
    }
    return 1;
}
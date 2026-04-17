/*
** Project  -  my_teams
** Date     -  April 14st 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include <uuid/uuid.h>

int cmd_subscribed(char **args, int client_socket)
{
    if (current_user.is_logged == false) {
        printf("-> User not logged.\n");
        return 1;
    }

    char cmd[1024];
    if (args[1] == NULL) {
        // List all teams
        snprintf(cmd, sizeof(cmd), "SUBD\r\n");
    } else {
        // List all users
        snprintf(cmd, sizeof(cmd), "SUBD \"%s\"\r\n", args[1]);
    }

    write(client_socket, cmd, strlen(cmd));

    char response[4096];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    if (res_read <= 0) return 1;
    response[res_read] = '\0';

    char *line = strtok(response, "\n");
    while (line != NULL) {
        if (strncmp(line, "243", 3) == 0) {
            char uuid[UUID_LENGTH];
            char name[MAX_NAME_LENGTH];
            char desc[MAX_DESCRIPTION_LENGTH];
            sscanf(line + 4, "%s \"%[^\"]\" \"%[^\"]\"", uuid, name, desc);
            client_print_teams(uuid, name, desc);
        } else if (strncmp(line, "244", 3) == 0) {
            char uuid[UUID_LENGTH];
            char name[MAX_NAME_LENGTH];
            int status;
            sscanf(line + 4, "%s \"%[^\"]\" %d", uuid, name, &status);
            client_print_users(uuid, name, status);
        } else if (strncmp(line, "440", 3) == 0) {
            client_error_unknown_team(line + 4);
        }
        line = strtok(NULL, "\n");
    }
    return 1;
}
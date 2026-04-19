/*
** Project  -  my_teams
** Date     -  April 14st 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include "commands.h"

int cmd_subscribed(char **args, int client_socket)
{
    if (current_user.is_logged == false) {
        printf("-> User not logged.\n");
        return 1;
    }

    char cmd[1024];
    if (args[1] == NULL) {
        // List all teams
        snprintf(cmd, sizeof(cmd), "SUBL\r\n");
    } else {
        // List all users
        snprintf(cmd, sizeof(cmd), "SUBU \"%s\"\r\n", args[1]);
    }

    write(client_socket, cmd, strlen(cmd));

    char response[4096];
    int res_read;

    while (1) {
        res_read = read(client_socket, response, sizeof(response) - 1);
        if (res_read <= 0)
            return cmd_logout(args, client_socket);
        response[res_read] = '\0';

        char *line = strtok(response, "\r\n");
        while (line != NULL) {
            if (strncmp(line, "240", 3) == 0) {
            } else if (strncmp(line, "250", 3) == 0) {
                return 1;
            } else if (strncmp(line, "243", 3) == 0) {
                char uuid[UUID_LENGTH] = {0};
                char name[MAX_NAME_LENGTH] = {0};
                char desc[MAX_DESCRIPTION_LENGTH] = {0};
                if (sscanf(line + 4, "%36s \"%31[^\"]\" \"%254[^\"]\"", uuid, name, desc) == 3)
                    client_print_teams(uuid, name, desc);
            } else if (strncmp(line, "244", 3) == 0) {
                char uuid[UUID_LENGTH] = {0};
                char name[MAX_NAME_LENGTH] = {0};
                int status = 0;
                if (sscanf(line + 4, "%36s \"%31[^\"]\" %d", uuid, name, &status) == 3)
                    client_print_users(uuid, name, status);
            } else if (strncmp(line, "440", 3) == 0) {
                client_error_unknown_team(line + 4);
                return 1;
            } else if (strncmp(line, "401", 3) == 0) {
                client_error_unauthorized();
                return 1;
            }
            line = strtok(NULL, "\r\n");
        }
    }

    return 1;
}
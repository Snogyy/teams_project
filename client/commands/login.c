/*
** Project  -  my_teams
** Date     -  March 31st 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include "commands.h"

int cmd_login(char **args, int client_socket)
{
    if (args[1] == NULL || args[2] != NULL) {
        printf("-> Usage: /login [username]\n");
        return 1;
    }
    if (strlen(args[1]) > MAX_NAME_LENGTH - 1) {
        printf("-> Username is too long\n");
        return 1;
    }

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "LOGI \"%s\"\r\n", args[1]);
    write(client_socket, cmd, strlen(cmd));

    // Wait respons
    char response[1024];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    // If server didnt respond logout user
    if (res_read <= 0)
        return cmd_logout(args, client_socket);
    response[res_read] = '\0';

    if (strncmp(response, "230 ", 4) == 0) {
        char user_uuid[UUID_LENGTH] = {0};
        char user_name[MAX_NAME_LENGTH] = {0};
        char *ptr = response + 4;

        if (sscanf(ptr, "%36s %31s", user_uuid, user_name) == 2) {
            current_user.is_logged = true;
            strcpy(current_user.uuid, user_uuid);
            strcpy(current_user.username, user_name);
            client_event_logged_in(user_uuid, user_name);
        }
    } else
        printf("-> Login failed\n");

    return 1;
}

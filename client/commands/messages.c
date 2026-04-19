/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../client.h"
#include "commands.h"

int cmd_messages(char **args, int client_socket)
{
    if (args[1] == NULL || args[2] != NULL) {
        printf("Usage: /messages \"user_uuid\"\n");
        return 0;
    }

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "MSGS \"%s\"\r\n", args[1]);
    write(client_socket, cmd, strlen(cmd));

    char response[4096];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    if (res_read <= 0)
        return cmd_logout(args, client_socket);
    response[res_read] = '\0';

    char *line = strtok(response, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "250", 3) == 0)
            return 1;
        else if (strncmp(line, "MSG ", 4) == 0) {
            char sender_uuid[UUID_LENGTH];
            long timestamp;
            char body[MAX_BODY_LENGTH];
            if (sscanf(line + 4, "%36s %ld \"%511[^\"]\"",
                    sender_uuid, &timestamp, body) == 3)
                client_private_message_print_messages(sender_uuid,
                    (time_t)timestamp, body);
        } else if (strncmp(line, "404", 3) == 0) {
            client_error_unknown_user(args[1]);
            return 1;
        } else if (strncmp(line, "401", 3) == 0) {
            client_error_unauthorized();
            return 0;
        }
        line = strtok(NULL, "\r\n");
    }
    return 0;
}
/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../client.h"
#include "commands.h"

int cmd_users(char **args, int client_socket)
{
    write(client_socket, "USRS\r\n", 6);

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
            } else if (strncmp(line, "USER ", 5) == 0) {
                char uuid[37], name[32];
                int status;
                if (sscanf(line + 5, "%36s %31s %d", uuid, name, &status) == 3)
                    client_print_users(uuid, name, status);
            } else if (strncmp(line, "401", 3) == 0) {
                client_error_unauthorized();
                return 1;
            } else {
                printf("%s\n", line);
            }
            line = strtok(NULL, "\r\n");
        }
    }
    return 1;
}
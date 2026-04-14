/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../server.h"

void signal_user(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;

    if (argument == NULL || strlen(argument) == 0) {
        generate_client_respons(actual_client_fd, find_reply(400));
        return;
    }

    char uuid[UUID_LENGTH] = {0};
    if (argument[0] == '"' && argument[strlen(argument) - 1] == '"')
        strncpy(uuid, argument + 1, strlen(argument) - 2);
    else
        strncpy(uuid, argument, UUID_LENGTH - 1);

    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].uuid, uuid) == 0) {
            char line[512];
            snprintf(line, sizeof(line), "USER %s %s %d",
                server.users[i].uuid,
                server.users[i].name,
                server.users[i].is_connected ? 1 : 0);
            generate_client_respons(actual_client_fd, line);
            generate_client_respons(actual_client_fd, find_reply(250));
            return;
        }
    }

    generate_client_respons(actual_client_fd, find_reply(404));
}
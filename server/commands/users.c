/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../server.h"

void signal_users(char *argument, int client_index)
{
    (void)argument;
    int actual_client_fd = server.pfd_list[client_index].fd;

    generate_client_respons(actual_client_fd, find_reply_server(240));

    for (int i = 0; i < server.nb_users; i++) {
        char line[512];
        snprintf(line, sizeof(line), "USER %s %s %d",
            server.users[i].uuid,
            server.users[i].name,
            server.users[i].is_connected ? 1 : 0);
        generate_client_respons(actual_client_fd, line);
    }

    generate_client_respons(actual_client_fd, find_reply_server(250));
}
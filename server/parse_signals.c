/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "server.h"

void parse_signals(char *received_buffer, int client_index)
{
    char *cmd = NULL;
    char *arg = NULL;
    char *space = NULL;

    received_buffer[strcspn(received_buffer, "\r\n")] = '\0';
    space = strchr(received_buffer, ' ');
    if (space != NULL) {
        *space = '\0';
        arg = space + 1;
        while (*arg == ' ')
            arg++;
        if (*arg == '\0')
            arg = NULL;
    }
    cmd = received_buffer;

    if (cmd == NULL || *cmd == '\0')
        return generate_client_respons(server.pfd_list[client_index].fd, find_reply_server(500));

    if (server.clients[client_index - 1].logged == false) {
        if (strcmp(cmd, "LOGI") != 0)
            return generate_client_respons(server.pfd_list[client_index].fd, find_reply_server(401));
    }

    for (int i = 0; signals[i].name != NULL; i++) {
        if (strcmp(cmd, signals[i].name) == 0) {
            signals[i].signal_func(arg, client_index);
            return;
        }
    }
    generate_client_respons(server.pfd_list[client_index].fd, find_reply_server(500));
}
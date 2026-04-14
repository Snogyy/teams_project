/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "server.h"

void parse_signals(char *received_buffer, int client_index)
{
    received_buffer[strcspn(received_buffer, "\r\n")] = '\0';
    char *cmd = strtok(received_buffer, " ");
    char *arg = strtok(NULL, "");

    if (cmd == NULL)
        return generate_client_respons(server.pfd_list[client_index].fd, "500 Unrecognized command");

    if (server.clients[client_index - 1].logged == false) {
        if (strcmp(cmd, "LOGI") != 0)
            return generate_client_respons(server.pfd_list[client_index].fd, "530 Not logged in.");
    }

    for (int i = 0; signals[i].name != NULL; i++) {
        if (strcmp(cmd, signals[i].name) == 0) {
            signals[i].signal_func(arg, client_index);
            return;
        }
    }
    generate_client_respons(server.pfd_list[client_index].fd, "500 Unrecognized command");
}
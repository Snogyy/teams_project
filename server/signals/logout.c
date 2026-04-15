/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

void signal_logout(char *argument, int client_index)
{
    client_info_t *actual_client = &server.clients[client_index - 1];

    actual_client->logged = false;
    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].uuid, actual_client->user_uuid) == 0) {
            server.users[i].is_connected = false;
            break;
        }
    }
    server_event_user_logged_out(actual_client->user_uuid);
}
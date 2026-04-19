/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

void signal_logout(char *argument, int client_index)
{
    (void)argument;
    int actual_client_fd = server.pfd_list[client_index].fd;
    client_info_t *actual_client = &server.clients[client_index - 1];
    char event[1024];

    if (!actual_client->logged) {
        generate_client_respons(actual_client_fd, find_reply_server(401));
        return;
    }

    snprintf(event, sizeof(event), "EVT LOGOUT \"%s\" \"%s\"", actual_client->user_uuid, actual_client->username);
    generate_all_logged_clients_event(event);

    actual_client->logged = false;
    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].uuid, actual_client->user_uuid) == 0) {
            server.users[i].is_connected = false;
            break;
        }
    }
    server_event_user_logged_out(actual_client->user_uuid);
}
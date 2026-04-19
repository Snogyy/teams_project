/*
** Project  -  my_teams
** Date     -  April 19th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

static void send_info_response(int fd, char *type, char *payload)
{
    char response[1024];

    snprintf(response, sizeof(response), "251 %s %s", type, payload);
    generate_client_respons(fd, response);
}

static int get_user_status(char *user_uuid)
{
    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].uuid, user_uuid) == 0)
            return server.users[i].is_connected ? 1 : 0;
    }
    return 0;
}

void signal_info(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];
    char payload[1024];

    if (argument != NULL && argument[0] != '\0')
        return generate_client_respons(actual_client_fd, find_reply_server(400));
    if (!is_valid_uuid(client->context, actual_client_fd))
        return;
    if (client->context.context_level != GLOBAL && !is_user_subscribed_to_team(client->context.team_uuid, client->user_uuid))
        return generate_client_respons(actual_client_fd, find_reply_server(403));

    if (client->context.context_level == GLOBAL) {
        int status = get_user_status(client->user_uuid);

        snprintf(payload, sizeof(payload), "\"%s\" \"%s\" %d", client->user_uuid, client->username, status);
        send_info_response(actual_client_fd, "USER", payload);
        return;
    }

    if (client->context.context_level == TEAM) {
        for (int i = 0; i < server.nb_teams; i++) {
            if (strcmp(server.teams[i].uuid, client->context.team_uuid) == 0) {
                snprintf(payload, sizeof(payload), "\"%s\" \"%s\" \"%s\"", server.teams[i].uuid, server.teams[i].name, server.teams[i].description);
                send_info_response(actual_client_fd, "TEAM", payload);
                return;
            }
        }
    }

    if (client->context.context_level == CHANNEL) {
        for (int i = 0; i < server.nb_channels; i++) {
            if (strcmp(server.channels[i].uuid, client->context.channel_uuid) == 0) {
                snprintf(payload, sizeof(payload), "\"%s\" \"%s\" \"%s\"", server.channels[i].uuid, server.channels[i].name, server.channels[i].description);
                send_info_response(actual_client_fd, "CHANNEL", payload);
                return;
            }
        }
    }

    if (client->context.context_level == THREAD) {
        for (int i = 0; i < server.nb_threads; i++) {
            if (strcmp(server.threads[i].uuid, client->context.thread_uuid) == 0) {
                snprintf(payload, sizeof(payload), "\"%s\" \"%s\" %ld \"%s\" \"%s\"", server.threads[i].uuid, server.threads[i].user_uuid, (long)server.threads[i].timestamp, server.threads[i].title, server.threads[i].body);
                send_info_response(actual_client_fd, "THREAD", payload);
                return;
            }
        }
    }

    generate_client_respons(actual_client_fd, find_reply_server(404));
}

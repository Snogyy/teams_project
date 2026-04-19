/*
** Project  -  my_teams
** Date     -  April 16th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

static int actual_client_fd;
static client_info_t *client;

void send_list_response()
{
    char response[1024];

    if (!is_valid_uuid(client->context, actual_client_fd))
        return;
    if (client->context.context_level != GLOBAL && !is_user_subscribed_to_team(client->context.team_uuid, client->user_uuid))
        return generate_client_respons(actual_client_fd, find_reply_server(403));

    if (client->context.context_level == GLOBAL) {
        generate_client_respons(actual_client_fd, find_reply_server(240));
        for (int i = 0; i < server.nb_teams; i++) {
            snprintf(response, sizeof(response), "251 %s \"%s\" \"%s\" \"%s\"", "TEAM", server.teams[i].uuid, server.teams[i].name, server.teams[i].description);
            generate_client_respons(actual_client_fd, response);
        }
        generate_client_respons(actual_client_fd, find_reply_server(250));
    } else if (client->context.context_level == TEAM) {
        generate_client_respons(actual_client_fd, find_reply_server(240));
        for (int i = 0; i < server.nb_channels; i++) {
            if (strcmp(server.channels[i].team_uuid, client->context.team_uuid) == 0) {
                snprintf(response, sizeof(response), "251 %s \"%s\" \"%s\" \"%s\"", "CHANNEL", server.channels[i].uuid, server.channels[i].name, server.channels[i].description);
                generate_client_respons(actual_client_fd, response);
            }
        }
        generate_client_respons(actual_client_fd, find_reply_server(250));
    } else if (client->context.context_level == CHANNEL) {
        generate_client_respons(actual_client_fd, find_reply_server(240));
        for (int i = 0; i < server.nb_threads; i++) {
            if (strcmp(server.threads[i].channel_uuid, client->context.channel_uuid) == 0) {
                snprintf(response, sizeof(response), "251 %s \"%s\" \"%s\" %ld \"%s\" \"%s\"", "THREAD", server.threads[i].uuid, server.threads[i].user_uuid, server.threads[i].timestamp, server.threads[i].title, server.threads[i].body);
                generate_client_respons(actual_client_fd, response);
            }
        }
        generate_client_respons(actual_client_fd, find_reply_server(250));
    } else if (client->context.context_level == THREAD) {
        generate_client_respons(actual_client_fd, find_reply_server(240));
        for (int i = 0; i < server.nb_replies; i++) {
            if (strcmp(server.replies[i].thread_uuid, client->context.thread_uuid) == 0) {
                snprintf(response, sizeof(response), "251 %s \"%s\" \"%s\" %ld \"%s\"", "REPLY", server.replies[i].thread_uuid, server.replies[i].user_uuid, server.replies[i].timestamp, server.replies[i].body);
                generate_client_respons(actual_client_fd, response);
            }
        }
        generate_client_respons(actual_client_fd, find_reply_server(250));
    }
}

void signal_list(char *argument, int client_index)
{
    actual_client_fd = server.pfd_list[client_index].fd;
    client = &server.clients[client_index - 1];

    send_list_response();
}

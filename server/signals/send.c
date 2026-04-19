/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../server.h"

void signal_send(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];

    if (argument == NULL || strlen(argument) == 0) {
        generate_client_respons(actual_client_fd, find_reply_server(400));
        return;
    }

    char receiver_uuid[UUID_LENGTH] = {0};
    char body[MAX_BODY_LENGTH] = {0};

    if (argument[0] != '"') {
        generate_client_respons(actual_client_fd, find_reply_server(400));
        return;
    }
    char *uuid_end = strchr(argument + 1, '"');
    if (!uuid_end) {
        generate_client_respons(actual_client_fd, find_reply_server(400));
        return;
    }
    strncpy(receiver_uuid, argument + 1, uuid_end - argument - 1);

    char *body_start = strchr(uuid_end + 1, '"');
    if (!body_start) {
        generate_client_respons(actual_client_fd, find_reply_server(400));
        return;
    }
    body_start++;
    char *body_end = strrchr(body_start, '"');
    if (!body_end || body_end == body_start - 1) {
        generate_client_respons(actual_client_fd, find_reply_server(400));
        return;
    }
    strncpy(body, body_start, body_end - body_start);

    if (strlen(body) > MAX_BODY_LENGTH - 1) {
        generate_client_respons(actual_client_fd, find_reply_server(413));
        return;
    }

    int receiver_index = -1;
    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].uuid, receiver_uuid) == 0) {
            receiver_index = i;
            break;
        }
    }
    if (receiver_index == -1) {
        generate_client_respons(actual_client_fd, find_reply_server(404));
        return;
    }

    server_event_private_message_sended(client->user_uuid, receiver_uuid, body);

    private_message_t *msg = &server.messages[server.nb_messages];
    strncpy(msg->sender_uuid, client->user_uuid, UUID_LENGTH - 1);
    strncpy(msg->receiver_uuid, receiver_uuid, UUID_LENGTH - 1);
    strncpy(msg->body, body, MAX_BODY_LENGTH - 1);
    msg->timestamp = time(NULL);
    server.nb_messages++;

    for (int i = 1; i < server.nb_clients; i++) {
        if (server.clients[i - 1].logged && strcmp(server.clients[i - 1].user_uuid, receiver_uuid) == 0) {
            char notif[512 + 100];
            snprintf(notif, sizeof(notif), "EVT PRIVATE_MESSAGE \"%s\" \"%s\"", client->user_uuid, body);
            generate_client_respons(server.pfd_list[i].fd, notif);
        }
    }

    generate_client_respons(actual_client_fd, find_reply_server(250));
}
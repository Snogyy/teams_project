/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Quentin Beyl
*/

#include "../server.h"

void signal_messages(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];

    if (argument == NULL || strlen(argument) == 0) {
        generate_client_respons(actual_client_fd, find_reply_server(400));
        return;
    }

    char target_uuid[UUID_LENGTH] = {0};
    if (argument[0] == '"' && argument[strlen(argument) - 1] == '"')
        strncpy(target_uuid, argument + 1, strlen(argument) - 2);
    else
        strncpy(target_uuid, argument, UUID_LENGTH - 1);

    int found = 0;
    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].uuid, target_uuid) == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        generate_client_respons(actual_client_fd, find_reply_server(404));
        return;
    }

    generate_client_respons(actual_client_fd, find_reply_server(240));

    for (int i = 0; i < server.nb_messages; i++) {

        private_message_t *msg = &server.messages[i];
        if ((strcmp(msg->sender_uuid, client->user_uuid) == 0 &&
             strcmp(msg->receiver_uuid, target_uuid) == 0) ||
            (strcmp(msg->sender_uuid, target_uuid) == 0 &&
             strcmp(msg->receiver_uuid, client->user_uuid) == 0)) {
            char line[MAX_BODY_LENGTH + 100];
            snprintf(line, sizeof(line), "MSG %s %ld \"%s\"",
                msg->sender_uuid, msg->timestamp, msg->body);
            generate_client_respons(actual_client_fd, line);
        }
    }

    generate_client_respons(actual_client_fd, find_reply_server(250));
}
/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** connection
*/

#include "../server.h"

void signal_login(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];

    if (argument == NULL || strlen(argument) == 0) {
        generate_client_respons(actual_client_fd, "400 Missing username");
        return;
    }

    // Remove quotes
    char username[MAX_NAME_LENGTH] = {0};
    if (argument[0] == '"' && argument[strlen(argument) - 1] == '"') {
        strncpy(username, argument + 1, strlen(argument) - 2);
    } else {
        strncpy(username, argument, MAX_NAME_LENGTH - 1);
    }

    // Check if user exists
    int user_index = -1;
    for (int i = 0; i < server.nb_users; i++) {
        if (strcmp(server.users[i].name, username) == 0) {
            user_index = i;
            break;
        }
    }

    // User exists
    if (user_index != -1) {
        if (server.users[user_index].is_connected) {
            generate_client_respons(actual_client_fd, "430 User already connected");
            return;
        }

        server.users[user_index].is_connected = true;
        strcpy(client->user_uuid, server.users[user_index].uuid);
        strcpy(client->username, username);
        client->logged = true;

        server_event_user_logged_in(client->user_uuid);
        char response[1024];
        sprintf(response, "230 %s %s", client->user_uuid, username);
        generate_client_respons(actual_client_fd, response);
    // Create user
    } else {
        if (server.nb_users >= MAX_CLIENTS) {
            generate_client_respons(actual_client_fd, "500 Server full");
            return;
        }
        user_index = server.nb_users;

        uuid_t id;
        uuid_generate(id);
        uuid_unparse_lower(id, server.users[user_index].uuid);
        strcpy(server.users[user_index].name, username);
        server.users[user_index].is_connected = true;
        server.nb_users++;
        strcpy(client->user_uuid, server.users[user_index].uuid);
        strcpy(client->username, username);
        client->logged = true;

        server_event_user_created(client->user_uuid, username);
        server_event_user_logged_in(client->user_uuid);
        char response[1024];
        sprintf(response, "230 %s %s", client->user_uuid, username);
        generate_client_respons(actual_client_fd, response);
    }
}

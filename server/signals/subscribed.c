/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

static void list_user_subscribed_teams(int client_fd, client_info_t *client)
{
    for (int i = 0; i < server.nb_teams; i++) {
        for (int j = 0; j < server.teams[i].nb_subscribers; j++) {
            if (strcmp(server.teams[i].subscribers[j], client->user_uuid) == 0) {
                char msg[1024];
                sprintf(msg, "243 %s \"%s\" \"%s\"",
                        server.teams[i].uuid,
                        server.teams[i].name,
                        server.teams[i].description);
                append_to_client_buffer(client_fd, msg);
                break; // Found the user  team
            }
        }
    }
}

static void list_team_subscribers(int client_fd, char *team_uuid)
{
    int t_index = -1;

    for (int i = 0; i < server.nb_teams; i++) {
        if (strcmp(server.teams[i].uuid, team_uuid) == 0) {
            t_index = i;
            break;
        }
    }
    if (t_index == -1) {
        char err[1024];
        sprintf(err, "440 %s", team_uuid);
        generate_client_respons(client_fd, err);
        return;
    }
    for (int i = 0; i < server.teams[t_index].nb_subscribers; i++) {
        char *s_uuid = server.teams[t_index].subscribers[i];
        for (int u = 0; u < server.nb_users; u++) {
            if (strcmp(server.users[u].uuid, s_uuid) == 0) {
                char msg[1024];
                sprintf(msg, "244 %s \"%s\" %d",
                        server.users[u].uuid,
                        server.users[u].name,
                        server.users[u].is_connected ? 1 : 0);
                append_to_client_buffer(client_fd, msg);
                break;
            }
        }
    }
}

void signal_subscribed(char *argument, int client_index)
{
    int client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];

    if (client->logged == false) {
        generate_client_respons(client_fd, "401 Unauthorized");
        return;
    }
    // No argument
    if (argument == NULL || argument[0] == '\0' || argument[0] == '\r') {
        list_user_subscribed_teams(client_fd, client);
        return;
    }

    // UUID gived
    char team_uuid[UUID_LENGTH] = {0};
    if (argument[0] == '"') {
        size_t len = strlen(argument);
        if (len > 2) {
            strncpy(team_uuid, argument + 1, len - 2);
        }
    } else {
        strncpy(team_uuid, argument, UUID_LENGTH - 1);
    }

    list_team_subscribers(client_fd, team_uuid);
}
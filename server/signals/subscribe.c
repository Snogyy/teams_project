/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

void signal_subscribe(char *argument, int client_index)
{
    int client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];

    if (client->logged == false) {
        generate_client_respons(client_fd, "401 Unauthorized");
        return;
    }

    // Remove quotes
    char team_uuid[UUID_LENGTH] = {0};
    if (argument[0] == '"') {
        strncpy(team_uuid, argument + 1, strlen(argument) - 2);
    } else {
        strncpy(team_uuid, argument, UUID_LENGTH - 1);
    }

    // Find team
    int team_index = -1;
    for (int i = 0; i < server.nb_teams; i++) {
        if (strcmp(server.teams[i].uuid, team_uuid) == 0) {
            team_index = i;
            break;
        }
    }

    if (team_index == -1) {
        char err[1024];
        sprintf(err, "440 %s", team_uuid); // Team not found
        generate_client_respons(client_fd, err);
        return;
    }

    // Already subscribed ?
    for (int i = 0; i < server.teams[team_index].nb_subscribers; i++) {
        if (strcmp(server.teams[team_index].subscribers[i], client->user_uuid) == 0) {
            generate_client_respons(client_fd, "431 Already subscribed");
            return;
        }
    }

    // Add subscribe
    int new_sub = server.teams[team_index].nb_subscribers;
    strcpy(server.teams[team_index].subscribers[new_sub], client->user_uuid);
    server.teams[team_index].nb_subscribers++;

    // Save
    save_db();
    server_event_user_subscribed(team_uuid, client->user_uuid);

    char response[1024];
    sprintf(response, "240 %s %s", client->user_uuid, team_uuid);
    generate_client_respons(client_fd, response);
}
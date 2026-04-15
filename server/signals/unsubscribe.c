/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

void signal_unsubscribe(char *argument, int client_index)
{
    int client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];

    if (client->logged == false) {
        generate_client_respons(client_fd, "401 Unauthorized");
        return;
    }

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
        sprintf(err, "440 %s", team_uuid);
        generate_client_respons(client_fd, err);
        return;
    }

    // Find user
    int user_pos = -1;
    for (int i = 0; i < server.teams[team_index].nb_subscribers; i++) {
        if (strcmp(server.teams[team_index].subscribers[i], client->user_uuid) == 0) {
            user_pos = i;
            break;
        }
    }

    if (user_pos == -1) {
        generate_client_respons(client_fd, "432 Not subscribed to this team");
        return;
    }

    // shifting to remove user
    for (int i = user_pos; i < server.teams[team_index].nb_subscribers - 1; i++) {
        strcpy(server.teams[team_index].subscribers[i], server.teams[team_index].subscribers[i + 1]);
    }
    server.teams[team_index].nb_subscribers--;

    //save the db
    save_db();
    server_event_user_unsubscribed(team_uuid, client->user_uuid);

    char response[1024];
    sprintf(response, "242 %s %s", client->user_uuid, team_uuid);
    generate_client_respons(client_fd, response);
}
/*
** Project  -  my_teams
** Date     -  April 15th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

void signal_use(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;
    client_info_t *client = &server.clients[client_index - 1];
    char *av[3];
    int ac = extract_args(argument, av, 3);

    if (ac == 0) {
        client->context.context_level = GLOBAL;
        strcpy(client->context.team_uuid, "");
        strcpy(client->context.channel_uuid, "");
        strcpy(client->context.thread_uuid, "");
        generate_client_respons(actual_client_fd, "USE GLOBAL");
    } else if (ac == 1) {
        client->context.context_level = TEAM;
        strcpy(client->context.team_uuid, av[0]);
        strcpy(client->context.channel_uuid, "");
        strcpy(client->context.thread_uuid, "");
        generate_client_respons(actual_client_fd, "USE TEAM");
    } else if (ac == 2) {
        client->context.context_level = CHANNEL;
        strcpy(client->context.team_uuid, av[0]);
        strcpy(client->context.channel_uuid, av[1]);
        strcpy(client->context.thread_uuid, "");
        generate_client_respons(actual_client_fd, "USE CHANNEL");
    } else if (ac == 3) {
        client->context.context_level = THREAD;
        strcpy(client->context.team_uuid, av[0]);
        strcpy(client->context.channel_uuid, av[1]);
        strcpy(client->context.thread_uuid, av[2]);
        generate_client_respons(actual_client_fd, "USE THREAD");
    }

}
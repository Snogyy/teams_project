/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "server.h"
#include "signals/signals.h"

server_struct_t server;

signals_t signals[] = {
    {"LOGI", signal_login},
    {"LOGO", signal_logout},
    {"SUB", signal_subscribe},
    {"USUB", signal_unsubscribe},
    {"SUBL", signal_subscribed_list},
    {"SUBU", signal_subscribed_users},
    {"USRS", signal_users},
    {"USER", signal_user},
    {"SEND", signal_send},
    {"MSGS", signal_messages},
    {"USE", signal_use},
    {"CREA", signal_create},
    {"LIST", signal_list},
    {NULL, NULL}
};

reply_server_t replies[] = {
    {200, "200 Command okay / Context successfully updated."},
    {220, "220 Service ready for new user."},
    {221, "221 Service closing control connection."},
    {230, "230 User logged in, proceed."},
    {240, "240 List/Info payload follows."},
    {250, "250 Requested action okay, completed."},
    {251, "251 Requested item created successfully."},
    {400, "400 Bad Request."},
    {401, "401 Unauthorized."},
    {403, "403 Forbidden."},
    {404, "404 Not Found."},
    {413, "413 Payload Too Large."},
    {500, "500 Syntax error, command unrecognized."},
    {550, "550 Internal server error / Database failure."},
    {560, "560 Server full."},
    {0, NULL}
};

void append_to_client_buffer(int actual_fd, char *message)
{
    for (int i = 1; i < server.nb_clients; i++) {
        if (server.pfd_list[i].fd == actual_fd) {
            if (server.clients[i - 1].write_len + strlen(message) < sizeof(server.clients[i - 1].write_buffer)) {
                strcpy(server.clients[i - 1].write_buffer + server.clients[i - 1].write_len, message);
                server.clients[i - 1].write_len += strlen(message);
                server.pfd_list[i].events |= POLLOUT;
            }
            return;
        }
    }
}

void generate_client_respons(int fd, char *respons)
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s\r\n", respons);
    append_to_client_buffer(fd, buffer);
}

void generate_all_logged_clients_event(char *event_message)
{
    for (int i = 1; i < server.nb_clients; i++) {
        if (server.clients[i - 1].logged)
            generate_client_respons(server.pfd_list[i].fd, event_message);
    }
}

void generate_team_subscribers_event(char *team_uuid, char *event_message)
{
    for (int i = 0; i < server.nb_teams; i++) {
        if (strcmp(server.teams[i].uuid, team_uuid) == 0) {
            for (int j = 0; j < server.teams[i].nb_subscribers; j++) {
                for (int k = 1; k < server.nb_clients; k++) {
                    if (server.clients[k - 1].logged && strcmp(server.clients[k - 1].user_uuid, server.teams[i].subscribers[j]) == 0)
                        generate_client_respons(server.pfd_list[k].fd, event_message);
                }
            }
            return;
        }
    }
}

char *find_reply_server(int reply_nb)
{
    for (int i = 0; replies[i].code != 0; i++) {
        if (replies[i].code == reply_nb)
            return replies[i].message;
    }
    return NULL;
}

int extract_args(char *argument, char **av, int max_args)
{
    char *cursor = argument;
    int ac = 0;

    if (argument == NULL)
        return 0;
    while (*cursor != '\0') {
        while (*cursor == ' ')
            cursor++;
        if (*cursor == '\0')
            break;
        if (*cursor != '"' || ac >= max_args)
            return -1;
        cursor++;
        av[ac++] = cursor;
        cursor = strchr(cursor, '"');
        if (cursor == NULL)
            return -1;
        *cursor = '\0';
        cursor++;
    }
    return ac;
}

bool is_user_subscribed_to_team(char *team_uuid, char *user_uuid)
{
    for (int i = 0; i < server.nb_teams; i++) {
        if (strcmp(server.teams[i].uuid, team_uuid) == 0) {
            for (int j = 0; j < server.teams[i].nb_subscribers; j++) {
                if (strcmp(server.teams[i].subscribers[j], user_uuid) == 0)
                    return true;
            }
            return false;
        }
    }
    return false;
}

// Verify if uuid enter in /use command is valid
bool is_valid_uuid(client_context_t context, int actual_client_fd)
{
    bool team_valid = false;
    bool channel_valid = false;
    bool thread_valid = false;

    if (context.team_uuid[0] != '\0') {
        for (int i = 0; i < server.nb_teams; i++) {
            if (strcmp(server.teams[i].uuid, context.team_uuid) == 0) {
                team_valid = true;
                break;
            }
        }
        if (!team_valid) {
            generate_client_respons(actual_client_fd, "404 TEAM Not Found");
            return false;
        }
    }
    if (context.channel_uuid[0] != '\0') {
        for (int i = 0; i < server.nb_channels; i++) {
            if (strcmp(server.channels[i].uuid, context.channel_uuid) == 0) {
                channel_valid = true;
                break;
            }
        }
        if (!channel_valid) {
            generate_client_respons(actual_client_fd, "404 CHANNEL Not Found");
            return false;
        }
    }
    if (context.thread_uuid[0] != '\0') {
        for (int i = 0; i < server.nb_threads; i++) {
            if (strcmp(server.threads[i].uuid, context.thread_uuid) == 0) {
                thread_valid = true;
                break;
            }
        }
        if (!thread_valid) {
            generate_client_respons(actual_client_fd, "404 THREAD Not Found");
            return false;
        }
    }

    return true;
}

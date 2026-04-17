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
    {"USRS", signal_users},
    {"USER", signal_user},
    {"SEND", signal_send},
    {"MSGS", signal_messages},
    {"USE", signal_use},
    {"CREA", signal_create},
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
    {404, "404 Not Found."},
    {413, "413 Payload Too Large."},
    {430, "430 User already connected."},
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

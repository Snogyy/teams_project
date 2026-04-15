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
    {"SUBB", signal_subscribe},
    {"UNSU", signal_unsubscribe},
    {"LOGO", signal_logout},
    {"USRS", signal_users},
    {"USER", signal_user},
    {"SEND", signal_send},
    {"MSGS", signal_messages},
    {NULL, NULL}
};

reply_t replies[] = {
    {120, "120 Service ready in nnn minutes."},
    {150, "150 File status okay; about to open data connection."},
    {200, "200 Command okay / Context successfully updated."},
    {214, "214 Help message (Shows available commands to the client)."},
    {220, "220 Service ready for new user."},
    {221, "221 Service closing control connection (Logged out)."},
    {230, "230 User logged in, proceed."},
    {240, "240 List/Info payload follows (Used before sending USRS, SUBL, LIST payloads)."},
    {250, "250 Requested action okay, completed (e.g., Message Sent, Valid Subscription)."},
    {251, "251 Requested item created successfully (e.g., Team, Channel, Thread, Reply)."},
    {300, "300 Pending further action."},
    {400, "400 Bad Request (Missing quotes, invalid syntax, missing args)."},
    {401, "401 Unauthorized (Client is not logged in)."},
    {403, "403 Forbidden (User is logged in but lacks rights, e.g. not subscribed to a team)."},
    {404, "404 Not Found (Requested UUID for user, team, channel, or thread does not exist)."},
    {413, "413 Payload Too Large (e.g., name > 32 chars, desc > 255 chars, body > 512 chars)."},
    {500, "500 Syntax error, command unrecognized."},
    {501, "501 Syntax error in parameters or arguments."},
    {503, "503 Bad sequence of commands."},
    {550, "550 Internal server error / Database failure."},
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

char *find_reply(int reply_nb)
{
    for (int i = 0; replies[i].code != 0; i++) {
        if (replies[i].code == reply_nb)
            return replies[i].message;
    }
    return NULL;
}

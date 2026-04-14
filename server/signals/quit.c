/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** quit
*/

#include "../server.h"

void signal_quit(char *argument, int client_index)
{
    int actual_client_fd = server.pfd_list[client_index].fd;
    server.clients[client_index - 1].logged = false;
    memset(server.clients[client_index - 1].username, 0, MAX_NAME_LENGTH);
    generate_client_respons(actual_client_fd, find_reply(221));
}

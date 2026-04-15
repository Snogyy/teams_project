/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"

int cmd_logout(char **args, int client_socket)
{
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "LOGO \r\n");
    write(client_socket, cmd, strlen(cmd));
    client_event_logged_out(current_user.uuid, current_user.username);
    return -1;
}

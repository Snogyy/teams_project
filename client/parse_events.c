/*
** Project  -  my_teams
** Date     -  April 19th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "client.h"

void handle_login_event(char *line)
{
    char user_uuid[UUID_LENGTH] = {0};
    char user_name[MAX_NAME_LENGTH] = {0};

    if (sscanf(line, "EVT LOGIN \"%36[^\"]\" \"%31[^\"]\"", user_uuid, user_name) == 2 && strcmp(current_user.uuid, user_uuid) == 0)
        client_event_logged_in(user_uuid, user_name);
}

void handle_logout_event(char *line)
{
    char user_uuid[UUID_LENGTH] = {0};
    char user_name[MAX_NAME_LENGTH] = {0};

    if (sscanf(line, "EVT LOGOUT \"%36[^\"]\" \"%31[^\"]\"", user_uuid, user_name) == 2 && strcmp(current_user.uuid, user_uuid) == 0)
        client_event_logged_out(user_uuid, user_name);
}

void parse_events(char *server_mess)
{
    char *line = strtok(server_mess, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "EVT LOGIN ", 10) == 0)
            handle_login_event(line);
        else if (strncmp(line, "EVT LOGOUT ", 11) == 0)
            handle_logout_event(line);
        line = strtok(NULL, "\r\n");
    }
}

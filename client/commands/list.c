/*
** Project  -  my_teams
** Date     -  April 19th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include "commands.h"

void handle_list_response(char *line)
{
    char type[16] = {0};
    if (sscanf(line + 4, "%15s", type) != 1)
        return;

    if (strcmp(type, "TEAM") == 0) {
        char team_uuid[UUID_LENGTH] = {0};
        char team_name[MAX_NAME_LENGTH] = {0};
        char team_description[MAX_DESCRIPTION_LENGTH] = {0};
        if (sscanf(line + 9, "\"%36[^\"]\" \"%31[^\"]\" \"%254[^\"]\"", team_uuid, team_name, team_description) == 3)
            client_print_team(team_uuid, team_name, team_description);
    } else if (strcmp(type, "CHANNEL") == 0) {
        char channel_uuid[UUID_LENGTH] = {0};
        char channel_name[MAX_NAME_LENGTH] = {0};
        char channel_description[MAX_DESCRIPTION_LENGTH] = {0};
        if (sscanf(line + 12, "\"%36[^\"]\" \"%31[^\"]\" \"%254[^\"]\"", channel_uuid, channel_name, channel_description) == 3)
            client_print_channel(channel_uuid, channel_name, channel_description);
    } else if (strcmp(type, "THREAD") == 0) {
        char thread_uuid[UUID_LENGTH] = {0};
        char user_uuid[UUID_LENGTH] = {0};
        long thread_timestamp = 0;
        char thread_title[MAX_NAME_LENGTH] = {0};
        char thread_body[MAX_BODY_LENGTH] = {0};
        if (sscanf(line + 11, "\"%36[^\"]\" \"%36[^\"]\" %ld \"%31[^\"]\" \"%511[^\"]\"", thread_uuid, user_uuid, &thread_timestamp, thread_title, thread_body) == 5)
            client_print_thread(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body);
    } else if (strcmp(type, "REPLY") == 0) {
        char thread_uuid[UUID_LENGTH] = {0};
        char user_uuid[UUID_LENGTH] = {0};
        long reply_timestamp = 0;
        char reply_body[MAX_BODY_LENGTH] = {0};
        if (sscanf(line + 10, "\"%36[^\"]\" \"%36[^\"]\" %ld \"%511[^\"]\"", thread_uuid, user_uuid, &reply_timestamp, reply_body) == 4)
            client_thread_print_replies(thread_uuid, user_uuid, reply_timestamp, reply_body);
    }
}

int cmd_list(char **args, int client_socket)
{
    char cmd[1024];

    if (args[1] != NULL) {
        printf("-> Usage: /list\n");
        return 1;
    }

    snprintf(cmd, sizeof(cmd), "LIST\r\n");
    write(client_socket, cmd, strlen(cmd));

    char response[4096];
    int res_read;

    while (1) {
        res_read = read(client_socket, response, sizeof(response) - 1);
        if (res_read <= 0)
            return cmd_logout(args, client_socket);
        response[res_read] = '\0';

        char *line = strtok(response, "\r\n");
        while (line != NULL) {
            if (strncmp(line, "240", 3) == 0) {
            } else if (strncmp(line, "250", 3) == 0) {
                return 1;
            } else if (strncmp(line, "251 ", 4) == 0) {
                handle_list_response(line);
            } else if (strncmp(line, "403", 3) == 0) {
                client_error_unauthorized();
                return 1;
            } else if (strncmp(line, "404", 3) == 0) {
                display_notfound_error(line);
                return 1;
            }
            line = strtok(NULL, "\r\n");
        }
    }

    return 1;
}

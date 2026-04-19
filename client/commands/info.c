/*
** Project  -  my_teams
** Date     -  April 19th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include "commands.h"

static void handle_info_response(char *line)
{
    char type[16] = {0};

    if (sscanf(line + 4, "%15s", type) != 1)
        return;

    if (strcmp(type, "USER") == 0) {
        char user_uuid[UUID_LENGTH] = {0};
        char user_name[MAX_NAME_LENGTH] = {0};
        int user_status = 0;
        if (sscanf(line + 9, "\"%36[^\"]\" \"%31[^\"]\" %d", user_uuid, user_name, &user_status) == 3)
            client_print_user(user_uuid, user_name, user_status);
    } else if (strcmp(type, "TEAM") == 0) {
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
            client_print_thread(thread_uuid, user_uuid, (time_t)thread_timestamp, thread_title, thread_body);
    }
}

int cmd_info(char **args, int client_socket)
{
    char response[1024];
    int res_read;

    if (args[1] != NULL) {
        printf("-> Usage: /info\n");
        return 1;
    }

    write(client_socket, "INFO\r\n", 6);

    while (1) {
        res_read = read(client_socket, response, sizeof(response) - 1);
        if (res_read <= 0)
            return cmd_logout(args, client_socket);
        response[res_read] = '\0';

        char *line = strtok(response, "\r\n");
        while (line != NULL) {
            if (strncmp(line, "251 ", 4) == 0) {
                handle_info_response(line);
                return 1;
            }
            if (strncmp(line, "403", 3) == 0) {
                client_error_unauthorized();
                return 1;
            }
            if (strncmp(line, "404", 3) == 0) {
                display_notfound_error(line);
                return 1;
            }
            line = strtok(NULL, "\r\n");
        }
    }
}

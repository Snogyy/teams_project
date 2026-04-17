/*
** Project  -  my_teams
** Date     -  April 16th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"
#include "commands.h"

static void display_approriate_error(char *line)
{
    char object_type[32] = {0};
    char object_uuid[UUID_LENGTH] = {0};
    int parsed_with_uuid = 0;

    parsed_with_uuid = sscanf(line, "404 %31s \"%36[^\"]\"", object_type, object_uuid);
    if (parsed_with_uuid != 2)
        parsed_with_uuid = sscanf(line, "404 %31s %36s", object_type, object_uuid);
    if (parsed_with_uuid < 1)
        return;

    if (strcmp(object_type, "TEAM") == 0) {
        if (parsed_with_uuid == 2)
            client_error_unknown_team(object_uuid);
        else
            client_error_unknown_team(current_context.team_uuid);
    } else if (strcmp(object_type, "CHANNEL") == 0) {
        if (parsed_with_uuid == 2)
            client_error_unknown_channel(object_uuid);
        else
            client_error_unknown_channel(current_context.channel_uuid);
    } else if (strcmp(object_type, "THREAD") == 0) {
        if (parsed_with_uuid == 2)
            client_error_unknown_thread(object_uuid);
        else
            client_error_unknown_thread(current_context.thread_uuid);
    } else if (strcmp(object_type, "USER") == 0) {
        if (parsed_with_uuid == 2)
            client_error_unknown_user(object_uuid);
        else
            client_error_unknown_user("");
    }
}

void handle_create_response(char *line)
{
    char type[16] = {0};
    if (sscanf(line + 4, "%15s", type) != 1)
        return;

    if (strcmp(type, "TEAM") == 0) {
        char team_uuid[UUID_LENGTH] = {0};
        char team_name[MAX_NAME_LENGTH] = {0};
        char team_description[MAX_DESCRIPTION_LENGTH] = {0};
        if (sscanf(line + 9, "\"%36[^\"]\" \"%31[^\"]\" \"%254[^\"]\"", team_uuid, team_name, team_description) == 3) {
            client_event_team_created(team_uuid, team_name, team_description);
            client_print_team_created(team_uuid, team_name, team_description);
        }
    } else if (strcmp(type, "CHANNEL") == 0) {
        char channel_uuid[UUID_LENGTH] = {0};
        char channel_name[MAX_NAME_LENGTH] = {0};
        char channel_description[MAX_DESCRIPTION_LENGTH] = {0};
        if (sscanf(line + 12, "\"%36[^\"]\" \"%31[^\"]\" \"%254[^\"]\"", channel_uuid, channel_name, channel_description) == 3) {
            client_event_channel_created(channel_uuid, channel_name, channel_description);
            client_print_channel_created(channel_uuid, channel_name, channel_description);
        }
    } else if (strcmp(type, "THREAD") == 0) {
        char thread_uuid[UUID_LENGTH] = {0};
        char user_uuid[UUID_LENGTH] = {0};
        long thread_timestamp = 0;
        char thread_title[MAX_NAME_LENGTH] = {0};
        char thread_body[MAX_BODY_LENGTH] = {0};
        if (sscanf(line + 11, "\"%36[^\"]\" \"%36[^\"]\" %ld \"%31[^\"]\" \"%511[^\"]\"", thread_uuid, user_uuid, &thread_timestamp, thread_title, thread_body) == 5) {
            client_event_thread_created(thread_uuid, user_uuid, (time_t)thread_timestamp, thread_title, thread_body);
            client_print_thread_created(thread_uuid, user_uuid, (time_t)thread_timestamp, thread_title, thread_body);
        }
    } else if (strcmp(type, "REPLY") == 0) {
        char thread_uuid[UUID_LENGTH] = {0};
        char user_uuid[UUID_LENGTH] = {0};
        long reply_timestamp = 0;
        char reply_body[MAX_BODY_LENGTH] = {0};
        if (sscanf(line + 10, "\"%36[^\"]\" \"%36[^\"]\" %ld \"%511[^\"]\"", thread_uuid, user_uuid, &reply_timestamp, reply_body) == 4)
            client_print_reply_created(thread_uuid, user_uuid,(time_t)reply_timestamp, reply_body);
    }
}

int cmd_create(char **args, int client_socket)
{
    char cmd[1024];

    if (args[1] == NULL) {
        printf("-> Usage: /create [”item_name”] [”item_description”]\n  Or /create [”comment_body”]");
        return 1;
    } else if (args[2] == NULL) {
        snprintf(cmd, sizeof(cmd), "CREA \"%s\"\r\n", args[1]);
        write(client_socket, cmd, strlen(cmd));
    } else {
        snprintf(cmd, sizeof(cmd), "CREA \"%s\" \"%s\"\r\n", args[1], args[2]);
        write(client_socket, cmd, strlen(cmd));
    }

    char response[1024];
    int res_read = read(client_socket, response, sizeof(response) - 1);
    if (res_read <= 0)
        return cmd_logout(args, client_socket);
    response[res_read] = '\0';

    char *line = strtok(response, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "251 ", 4) == 0)
            handle_create_response(line);
        else if (strncmp(line, "400 ALREADY_EXIST", 17) == 0)
            client_error_already_exist();
        else if (strncmp(line, "400", 3) == 0)
            printf("-> ERROR: Bad request.\n");
        else if (strncmp(line, "404", 3) == 0)
            display_approriate_error(line);
        line = strtok(NULL, "\r\n");
    }

    return 1;
}
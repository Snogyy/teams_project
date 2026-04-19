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

    if (sscanf(line, "EVT LOGIN \"%36[^\"]\" \"%31[^\"]\"", user_uuid, user_name) == 2) {
        printf("\n\r");
        client_event_logged_in(user_uuid, user_name);
        fflush(stdout);
    }
}

void handle_logout_event(char *line)
{
    char user_uuid[UUID_LENGTH] = {0};
    char user_name[MAX_NAME_LENGTH] = {0};

    if (sscanf(line, "EVT LOGOUT \"%36[^\"]\" \"%31[^\"]\"", user_uuid, user_name) == 2) {
        printf("\n\r");
        client_event_logged_out(user_uuid, user_name);
        fflush(stdout);
    }
}

void handle_private_message_event(char *line)
{
    char sender_uuid[UUID_LENGTH] = {0};
    char body[MAX_BODY_LENGTH] = {0};

    if (sscanf(line, "EVT PRIVATE_MESSAGE \"%36[^\"]\" \"%511[^\"]\"", sender_uuid, body) == 2) {
        printf("\n\r");
        client_event_private_message_received(sender_uuid, body);
        fflush(stdout);
    }
}

void handle_team_created_event(char *line)
{
    char team_uuid[UUID_LENGTH] = {0};
    char team_name[MAX_NAME_LENGTH] = {0};
    char team_description[MAX_DESCRIPTION_LENGTH] = {0};

    if (sscanf(line, "EVT TEAM_CREATED \"%36[^\"]\" \"%31[^\"]\" \"%254[^\"]\"", team_uuid, team_name, team_description) == 3) {
        printf("\n\r");
        client_event_team_created(team_uuid, team_name, team_description);
        fflush(stdout);
    }
}

void handle_channel_created_event(char *line)
{
    char channel_uuid[UUID_LENGTH] = {0};
    char channel_name[MAX_NAME_LENGTH] = {0};
    char channel_description[MAX_DESCRIPTION_LENGTH] = {0};

    if (sscanf(line, "EVT CHANNEL_CREATED \"%36[^\"]\" \"%31[^\"]\" \"%254[^\"]\"", channel_uuid, channel_name, channel_description) == 3) {
        printf("\n\r");
        client_event_channel_created(channel_uuid, channel_name, channel_description);
        fflush(stdout);
    }
}

void handle_thread_created_event(char *line)
{
    char thread_uuid[UUID_LENGTH] = {0};
    char user_uuid[UUID_LENGTH] = {0};
    long thread_timestamp = 0;
    char thread_title[MAX_NAME_LENGTH] = {0};
    char thread_body[MAX_BODY_LENGTH] = {0};

    if (sscanf(line, "EVT THREAD_CREATED \"%36[^\"]\" \"%36[^\"]\" %ld \"%31[^\"]\" \"%511[^\"]\"", thread_uuid, user_uuid, &thread_timestamp, thread_title, thread_body) == 5) {
        printf("\n\r");
        client_event_thread_created(thread_uuid, user_uuid, (time_t)thread_timestamp, thread_title, thread_body);
        fflush(stdout);
    }
}

void handle_reply_created_event(char *line)
{
    char thread_uuid[UUID_LENGTH] = {0};
    char user_uuid[UUID_LENGTH] = {0};
    long reply_timestamp = 0;
    char reply_body[MAX_BODY_LENGTH] = {0};

    if (sscanf(line, "EVT REPLY_CREATED \"%36[^\"]\" \"%36[^\"]\" %ld \"%511[^\"]\"", thread_uuid, user_uuid, &reply_timestamp, reply_body) == 4) {
        printf("\n\r");
        client_print_reply_created(thread_uuid, user_uuid, (time_t)reply_timestamp, reply_body);
        fflush(stdout);
    }
}

void parse_events(char *server_mess)
{
    char *line = strtok(server_mess, "\r\n");
    while (line != NULL) {
        if (strncmp(line, "EVT LOGIN ", 10) == 0)
            handle_login_event(line);
        else if (strncmp(line, "EVT LOGOUT ", 11) == 0)
            handle_logout_event(line);
        else if (strncmp(line, "EVT PRIVATE_MESSAGE ", 20) == 0)
            handle_private_message_event(line);
        else if (strncmp(line, "EVT TEAM_CREATED ", 17) == 0)
            handle_team_created_event(line);
        else if (strncmp(line, "EVT CHANNEL_CREATED ", 20) == 0)
            handle_channel_created_event(line);
        else if (strncmp(line, "EVT THREAD_CREATED ", 19) == 0)
            handle_thread_created_event(line);
        else if (strncmp(line, "EVT REPLY_CREATED ", 18) == 0)
            handle_reply_created_event(line);
        line = strtok(NULL, "\r\n");
    }
}

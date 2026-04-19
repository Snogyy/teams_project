/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "client.h"
#include "commands/commands.h"

user_t current_user;

client_context_t current_context = {
    .team_uuid = "",
    .channel_uuid = "",
    .thread_uuid = "",
    .context_level = GLOBAL
};

commands_t commands[] = {
    {"/login", cmd_login},
    {"/logout", cmd_logout},
    {"/help", cmd_help},
    {"/subscribe", cmd_subscribe},
    {"/unsubscribe", cmd_unsubscribe},
    {"/subscribed", cmd_subscribed},
    {"/users", cmd_users},
    {"/user",  cmd_user},
    {"/send",  cmd_send},
    {"/messages", cmd_messages},
    {"/use", cmd_use},
    {"/create", cmd_create},
    {"/list", cmd_list},
    {"/info", cmd_info},
    {NULL, NULL}
};

void display_notfound_error(char *line)
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

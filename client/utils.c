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
    {NULL, NULL}
};
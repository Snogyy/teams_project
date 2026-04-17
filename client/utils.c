/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "client.h"
#include "commands/commands.h"

current_user_t current_user;

commands_t commands[] = {
    {"/login", cmd_login},
    {"/logout", cmd_logout},
    {"/subscribe", cmd_subscribe},
    {"/unsubscribe", cmd_unsubscribe},
    {"/subscribed", cmd_subscribed},
    {"/help", cmd_help},
    {"/users", cmd_users},
    {"/user",  cmd_user},
    {"/send",  cmd_send},
    {"/messages", cmd_messages},
    {NULL, NULL}
};
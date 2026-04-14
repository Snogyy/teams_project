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
    {"/users", cmd_users},
    {NULL, NULL}
};
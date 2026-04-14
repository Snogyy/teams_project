/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <uuid/uuid.h>

#include "../libs/myteams/logging_client.h"

#ifndef INCLUDED_CLIENT_H
    #define INCLUDED_CLIENT_H

    #define MAX_NAME_LENGTH 32
    #define MAX_DESCRIPTION_LENGTH 255
    #define MAX_BODY_LENGTH 512
    #define UUID_LENGTH 37

    typedef struct {
        char uuid[UUID_LENGTH];
        char username[MAX_NAME_LENGTH];
        bool is_logged;
    } current_user_t;

    typedef struct {
        char team_uuid[UUID_LENGTH];
        char channel_uuid[UUID_LENGTH];
        char thread_uuid[UUID_LENGTH];
        int level; // 0: global, 1: team, 2: channel, 3: thread
    } client_context_t;

    typedef struct {
        char *name;
        void (*cmd_func)(char **args, int client_socket);
    } commands_t;

    void parse_commands(int client_socket);
    extern current_user_t current_user;
    extern commands_t commands[];

#endif

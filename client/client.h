/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include <poll.h>
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
    } user_t;

    enum CONTEXT_LEVEL {
        TEAM,
        CHANNEL,
        THREAD,
        GLOBAL
    };

    typedef struct {
        char team_uuid[UUID_LENGTH];
        char channel_uuid[UUID_LENGTH];
        char thread_uuid[UUID_LENGTH];
        enum CONTEXT_LEVEL context_level;
    } client_context_t;

    typedef struct {
        char *name;
        int (*cmd_func)(char **args, int client_socket);
    } commands_t;

    void parse_commands(int client_socket);
    extern user_t current_user;
    extern client_context_t current_context;
    extern commands_t commands[];

#endif

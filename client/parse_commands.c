/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "client.h"

int separate_args(char *cmd, char **args)
{
    int argc = 0;
    int quote_error = 0;

    char *ptr = cmd;
    while (*ptr == ' ') ptr++;

    char *cmd_start = ptr;
    while (*ptr != ' ' && *ptr != '\0') ptr++;

    if (*ptr == ' ') {
        *ptr = '\0';
        ptr++;
    }
    args[argc++] = cmd_start;

    while (*ptr != '\0' && argc < 10) {
        while (*ptr == ' ') ptr++;
        if (*ptr == '\0') break;

        if (*ptr != '"') {
            quote_error = 1;
            break;
        }
        ptr++;
        char *arg_start = ptr;

        while (*ptr != '"' && *ptr != '\0') ptr++;

        if (*ptr != '"') {
            quote_error = 1;
            break;
        }
        *ptr = '\0';
        ptr++;
        args[argc++] = arg_start;
    }

    if (quote_error) {
        printf("-> Arguments must takes double quotes.\n");
        return -1;
    }
    args[argc] = NULL;
    return argc;
}

void parse_commands(int client_socket)
{
    char cmd[512];
    int res_read;
    char *actual_arg;
    int argc;
    int command_found = 0;
    int command_res = 0;

    while (1) {
        printf("> ");
        fflush(stdout);

        res_read = read(0, cmd, sizeof(cmd)-1);
        if (res_read <= 0) break;
        if (cmd[res_read - 1] == '\n') cmd[res_read - 1] = '\0';

        // Separate command in args
        char *args[10];
        argc = separate_args(cmd, args);
        if (argc == -1) continue;

        // Exec command only if user is connected
        if (!current_user.is_logged && strcmp(args[0], "/login") != 0) {
            printf("-> You must be logged in to perform actions.\n");
            continue;
        }

        // Cannot logged in twice
        if (current_user.is_logged && strcmp(args[0], "/login") == 0) {
            printf("-> You are already logged in.\n");
            continue;
        }

        // Exec command
        for (int i = 0; commands[i].name != NULL; i++) {
            if (strcmp(args[0], commands[i].name) == 0) {
                command_res = commands[i].cmd_func(args, client_socket);
                command_found = 1;
                break;
            }
        }

        if (command_found == 0)
            printf("-> Unknown command\n");
        if (command_res == -1)
            break;
    }

}

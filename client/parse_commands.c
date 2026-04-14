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
        printf("Arguments must takes double quotes.\n");
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
    int command_exist;

    while (1) {
        printf("> ");
        fflush(stdout);

        res_read = read(0, cmd, sizeof(cmd)-1);
        if (res_read <= 0) break;
        if (cmd[res_read - 1] == '\n') cmd[res_read - 1] = '\0';

        if (strcmp(cmd, "/logout") == 0)
            break;

        // Separate command in args
        char *args[10];
        argc = separate_args(cmd, args);
        if (argc == -1) continue;

        // Exec command
        command_exist = 0;
        for (int i = 0; commands[i].name != NULL; i++) {
            if (strcmp(args[0], commands[i].name) == 0) {
                commands[i].cmd_func(args, client_socket);
                command_exist = 1;
                break;
            }
        }

        if (command_exist == 0)
            printf("Unknown command\n");
    }

}

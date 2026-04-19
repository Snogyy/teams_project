/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "client.h"

int is_command_complete(const char *buffer)
{
    // Incomplet
    if (!buffer || buffer[0] == '\0') return 0;

    // Complet
    if (strchr(buffer, '\n')) return 1;

    // Incomplet
    return 0;
}

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
    char cmd_buffer[512] = {0};
    int cmd_buffer_len = 0;

    char cmd[512];
    char server_mess[1024];
    int res_read;
    int argc;
    struct pollfd serv[2];
    int command_found = 0;
    int command_res = 0;

    serv[0].fd = STDIN_FILENO;
    serv[0].events = POLLIN;
    serv[1].fd = client_socket;
    serv[1].events = POLLIN;

    while (1) {
        int is_stdin_terminal = isatty(STDIN_FILENO);
        if (is_stdin_terminal) {
            printf("> ");
            fflush(stdout);
        }

        // lock
        if (poll(serv, 2, -1) == -1) {
            printf("-> poll\n");
            break;
        }

        // handle server message
        if (serv[1].revents & POLLIN) {
            res_read = read(client_socket, server_mess, sizeof(server_mess) - 1);
            if (res_read <= 0) {
                printf("\n-> Server disconnected.\n");
                break;
            }
            server_mess[res_read] = '\0';

            char *line = strtok(server_mess, "\r\n");
            while (line != NULL) {
                if (strncmp(line, "PMSG ", 5) == 0) {
                    char sender_uuid[UUID_LENGTH];
                    char body[MAX_BODY_LENGTH];
                    if (sscanf(line + 5, "%36s \"%511[^\"]\"", sender_uuid, body) == 2) {
                        printf("\r");
                        client_event_private_message_received(sender_uuid, body);
                        fflush(stdout);
                    }
                }
                line = strtok(NULL, "\r\n");
            }
            continue;
        }

        // parse command
        if (serv[0].revents & POLLIN) {
            res_read = read(0, cmd, sizeof(cmd) - 1);
            if (res_read <= 0) break;
            cmd[res_read] = '\0';

            // Append cmd to cmd_buffer
            if (cmd_buffer_len + res_read < sizeof(cmd_buffer) - 1) {
                strcat(cmd_buffer, cmd);
                cmd_buffer_len = strlen(cmd_buffer);
            } else {
                printf("-> Command too long.\n");
                cmd_buffer_len = 0;
                memset(cmd_buffer, 0, sizeof(cmd_buffer));
                continue;
            }

            // Check cmd_buffer
            if (is_command_complete(cmd_buffer) == 0) continue;

            // Remove trailing newline before parsing
            char *newline = strchr(cmd_buffer, '\n');
            if (newline) *newline = '\0';

            // Separate command in args
            char *args[10];
            argc = separate_args(cmd_buffer, args);
            if (argc == -1) {
                cmd_buffer_len = 0;
                memset(cmd_buffer, 0, sizeof(cmd_buffer));
                continue;
            }

            // Exec command only if user is connected
            if (!current_user.is_logged && strcmp(args[0], "/login") != 0) {
                client_error_unauthorized();
                cmd_buffer_len = 0;
                memset(cmd_buffer, 0, sizeof(cmd_buffer));
                continue;
            }

            // Cannot logged in twice
            if (current_user.is_logged && strcmp(args[0], "/login") == 0) {
                printf("-> You are already logged in.\n");
                cmd_buffer_len = 0;
                memset(cmd_buffer, 0, sizeof(cmd_buffer));
                continue;
            }

            // Exec command
            command_found = 0;
            for (int i = 0; commands[i].name != NULL; i++) {
                if (strcmp(args[0], commands[i].name) == 0) {
                    command_res = commands[i].cmd_func(args, client_socket);
                    command_found = 1;
                    break;
                }
            }

            if (command_found == 0)
                printf("-> Unknown command\n");

            // Clear buffer after processing
            cmd_buffer_len = 0;
            memset(cmd_buffer, 0, sizeof(cmd_buffer));

            if (command_res == -1)
                break;
        }
    }
}

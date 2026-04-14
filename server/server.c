/*
** EPITECH PROJECT, 2026
** bs-ftp
** File description:
** server
*/

#include "server.h"
#include <signal.h>

void handle_sigint(int sig)
{
    (void)sig;
    printf("Saving database and quitting...\n");
    save_db();

    for (int i = 0; i < server.nb_clients; i++) {
        if (server.pfd_list[i].fd > 0)
            close(server.pfd_list[i].fd);
    }

    exit(0);
}

int verify_args(int argc, char **argv)
{
    // Verify syntax execution
    if (argc != 2) {
        printf("USAGE: ./myteams_server port\n\n   port is the port number on which the server socket listens.\n");
        return 84;
    }
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0) {
        printf("USAGE: ./myteams_server port\n\n   port is the port number on which the server socket listens.\n");
        return 0;
    }

    // Verify port
    char *endptr;
    server.port = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || server.port <= 0) {
        printf("ERROR: Invalid port\n");
        return 84;
    }

    return 1;
}

int main(int argc, char **argv)
{
    // Verify if args are valids
    int res = verify_args(argc, argv);
    if (res == 84 || res == 0)
        return res;

    // Initializing the socket to listen for incoming connections
    server.server_address.sin_family = AF_INET;
    server.server_address.sin_port = htons(atoi(argv[1]));
    server.server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((server.server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("ERROR: Socket init failed\n");
        return 84;
    }
    if (bind(server.server_socket, (struct sockaddr *)&server.server_address, (unsigned int)sizeof(server.server_address)) < 0) {
        printf("ERROR: Bind failed\n");
        return 84;
    }
    if (listen(server.server_socket, 10) < 0) {
        printf("ERROR: Listen failed\n");
        return 84;
    }

    // Handle connections with poll
    server.pfd_list[0].fd = server.server_socket;
    server.pfd_list[0].events = POLLIN;
    server.nb_clients = 1;

    // Save data if Ctrl+C
    signal(SIGINT, handle_sigint);

    // Initializing data structure
    server.nb_users = 0;
    server.nb_teams = 0;
    server.nb_channels = 0;
    server.nb_threads = 0;
    load_db();

    while (1) {
        int res = poll(server.pfd_list, server.nb_clients, -1);
        if (res < 0) {
            printf("ERROR: Poll error\n");
            return 84;
        }

        receive_client_connections();
        handle_client();
    }

    save_db();
    for (int i = 0; i < server.nb_clients; i++) {
        if (server.pfd_list[i].fd > 0)
            close(server.pfd_list[i].fd);
    }
    close(server.server_socket);
    return 0;
}

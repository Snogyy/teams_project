/*
** Project  -  my_teams
** Date     -  March 30th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "client.h"

int main(int argc, char **argv)
{
    if (argc >= 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0)) {
        printf("USAGE: ./myteams_cli ip port\n\n   ip is the server ip address on which the server socket listens\n   port is the port number on which the server socket listens\n");
        return 0;
    }
    if (argc != 3) {
        printf("USAGE: ./myteams_cli ip port\n\n   ip is the server ip address on which the server socket listens\n   port is the port number on which the server socket listens\n");
        return 84;
    }

    // Initializing the socket for connect to the server
    int client_socket;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) {
        printf("ERROR: Invalid IP address\n");
        return 84;
    }
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("ERROR: Socket init failed\n");
        return 84;
    }

    // Connecting to the server
    if (connect(client_socket, (struct sockaddr *)&address, sizeof(address)) != 0) {
        printf("ERROR: Connection to server failure\n");
        return 84;
    }
    char buffer[1024];
    int res_read = read(client_socket, buffer, sizeof(buffer));
    if (res_read <= 0) {
        printf("ERROR: Read failed\n");
        return 84;
    }
    buffer[res_read] = '\0';
    printf("Server said: %s", buffer);

    parse_commands(client_socket);

    close(client_socket);
    return 0;
}

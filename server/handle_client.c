/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** handle_client
*/

#include "server.h"

void receive_client_connections()
{
    struct sockaddr_in server_address = server.server_address;

    if (server.pfd_list[0].revents & POLLIN) {
        socklen_t addrlen = sizeof(server_address);
        int client_socket = accept(server.server_socket, (struct sockaddr *)&server_address, &addrlen);

        if (client_socket < 0) {
            printf("Connection failed\n");
        } else if (server.nb_clients < MAX_CLIENTS) {
            server.clients[server.nb_clients - 1].data_transfer.data_socket = 0;
            strcpy(server.clients[server.nb_clients - 1].data_transfer.ip_str, "");
            server.clients[server.nb_clients - 1].data_transfer.port = -1;
            server.clients[server.nb_clients - 1].logged = false;
            server.clients[server.nb_clients - 1].write_len = 0;
            server.clients[server.nb_clients - 1].write_buffer[0] = '\0';
            server.clients[server.nb_clients - 1].read_len = 0;
            server.clients[server.nb_clients - 1].read_buffer[0] = '\0';
            server.pfd_list[server.nb_clients].fd = client_socket;
            server.pfd_list[server.nb_clients].events = POLLIN;
            server.pfd_list[server.nb_clients].revents = 0;
            server.nb_clients++;
            printf("Connection successful! Connection from %s:%i\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
            generate_client_respons(client_socket, find_reply(220));
        } else {
            write(client_socket, "Max clients reached.", strlen("Max clients reached."));
            close(client_socket);
        }
    }
}

void handle_client()
{
    for (int i = 1; i < server.nb_clients; i++) {
        if (server.pfd_list[i].revents & POLLIN) {
            client_info_t *client = &server.clients[i - 1];
            int free_space = sizeof(client->read_buffer) - client->read_len - 1;
            int res_read = read(server.pfd_list[i].fd, client->read_buffer + client->read_len, free_space);

            // If we cant read, client is offline
            if (res_read <= 0) {
                printf("Client %d disconnected\n", i);
                if (server.clients[i - 1].data_transfer.data_socket > 0)
                    close(server.clients[i - 1].data_transfer.data_socket);
                close(server.pfd_list[i].fd);
                for (int j = i; j < server.nb_clients - 1; j++) {
                    server.pfd_list[j] = server.pfd_list[j + 1];
                    server.clients[j - 1] = server.clients[j];
                }
                server.nb_clients--;
                i--;
                continue;
            }

            // Verify if the buffer have end of command separator
            client->read_len += res_read;
            client->read_buffer[client->read_len] = '\0';
            char *end_command;
            while ((end_command = strstr(client->read_buffer, "\r\n")) != NULL) {
                *end_command = '\0';
                parse_signals(client->read_buffer, i);
                char *next_command = end_command + 2;
                int next_command_len = client->read_len - (next_command - client->read_buffer);
                // Clear buffer and put the next command
                memmove(client->read_buffer, next_command, next_command_len);
                client->read_len = next_command_len;
            }
        }

        // Write buffer if event is POLLOUT
        if (server.pfd_list[i].revents & POLLOUT) {
            int write_len = server.clients[i - 1].write_len;
            if (write_len > 0 && write(server.pfd_list[i].fd, server.clients[i - 1].write_buffer, write_len) > 0) {
                server.clients[i - 1].write_len = 0;
                server.clients[i - 1].write_buffer[0] = '\0';
            }
            if (server.clients[i - 1].write_len == 0)
                server.pfd_list[i].events &= ~POLLOUT;
        }
    }
}

/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <uuid/uuid.h>

#include "../libs/myteams/logging_server.h"

#ifndef INCLUDED_SERVER_HPP
    #define INCLUDED_SERVER_HPP

    #define MAX_CLIENTS 1000

    #define MAX_NAME_LENGTH 32
    #define MAX_DESCRIPTION_LENGTH 255
    #define MAX_BODY_LENGTH 512
    #define UUID_LENGTH 37

    typedef struct {
        char uuid[UUID_LENGTH];
        char name[MAX_NAME_LENGTH];
        bool is_connected;
    } user_t;

    typedef struct {
        char sender_uuid[UUID_LENGTH];
        char receiver_uuid[UUID_LENGTH];
        char body[MAX_BODY_LENGTH];
        time_t timestamp;
    } private_message_t;

    typedef struct {
        char uuid[UUID_LENGTH];
        char name[MAX_NAME_LENGTH];
        char description[MAX_DESCRIPTION_LENGTH];
        char subscribers[MAX_CLIENTS][UUID_LENGTH];
        int nb_subscribers;
    } team_t;

    typedef struct {
        char uuid[UUID_LENGTH];
        char team_uuid[UUID_LENGTH];
        char name[MAX_NAME_LENGTH];
        char description[MAX_DESCRIPTION_LENGTH];
    } channel_t;

    typedef struct {
        char uuid[UUID_LENGTH];
        char channel_uuid[UUID_LENGTH];
        char author_uuid[UUID_LENGTH];
        char title[MAX_NAME_LENGTH];
        char body[MAX_BODY_LENGTH];
        time_t timestamp;
    } thread_t;

    typedef struct {
        int data_socket;
        char ip_str[16];
        int port;
    } data_transfer_t;

    typedef struct {
        char team_uuid[UUID_LENGTH];
        char channel_uuid[UUID_LENGTH];
        char thread_uuid[UUID_LENGTH];
        int level; // 0: global, 1: team, 2: channel, 3: thread
    } client_context_t;

    typedef struct {
        data_transfer_t data_transfer;
        char username[MAX_NAME_LENGTH];
        char user_uuid[UUID_LENGTH];
        bool logged;
        client_context_t context;
        char write_buffer[4096];
        int write_len;
        char read_buffer[4096];
        int read_len;
    } client_info_t;

    typedef struct {
        int server_socket;
        struct sockaddr_in server_address;
        long port;

        client_info_t clients[MAX_CLIENTS];
        struct pollfd pfd_list[MAX_CLIENTS * 2 + 1];
        int nb_clients;

        user_t users[MAX_CLIENTS];
        int nb_users;

        team_t teams[MAX_CLIENTS];
        int nb_teams;

        channel_t channels[MAX_CLIENTS * 10];
        int nb_channels;

        thread_t threads[MAX_CLIENTS * 10];
        int nb_threads;
    } server_struct_t;

     typedef struct {
        char *name;
        void (*signal_func)(char *argument, int actual_client_fd);
    } signals_t;

    typedef struct {
        int code;
        char *message;
    } reply_t;

    void receive_client_connections();
    void handle_client();
    void append_to_client_buffer(int fd, char *str);
    void generate_client_respons(int fd, char *respons);
    char *find_reply(int reply_nb);
    void parse_signals(char *received_buffer, int client_index);
    void signal_login(char *argument, int client_index);
    void signal_quit(char *argument, int client_index);
    int create_data_transfer_socket(int client_index);

    void save_db(void);
    void load_db(void);

    extern server_struct_t server;
    extern signals_t signals[];
    extern reply_t replies[];

#endif

/*
** Project  -  my_teams
** Date     -  April 13th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "server.h"

void save_db(void)
{
    FILE *fd = fopen("myteams_db.bin", "wb");
    if (!fd) {
        printf("-> ERROR: Could not open database file\n");
        return;
    }

    // Save users
    fwrite(&server.nb_users, sizeof(int), 1, fd);
    if (server.nb_users > 0)
        fwrite(server.users, sizeof(user_t), server.nb_users, fd);

    // Save teams
    fwrite(&server.nb_teams, sizeof(int), 1, fd);
    if (server.nb_teams > 0)
        fwrite(server.teams, sizeof(team_t), server.nb_teams, fd);

    // Save channels
    fwrite(&server.nb_channels, sizeof(int), 1, fd);
    if (server.nb_channels > 0)
        fwrite(server.channels, sizeof(channel_t), server.nb_channels, fd);

    // Save threads
    fwrite(&server.nb_threads, sizeof(int), 1, fd);
    if (server.nb_threads > 0)
        fwrite(server.threads, sizeof(thread_t), server.nb_threads, fd);

    // Save messages
    fwrite(&server.nb_messages, sizeof(int), 1, fd);
    if (server.nb_messages > 0)
        fwrite(server.messages, sizeof(private_message_t), server.nb_messages, fd);
    
    fclose(fd);
    printf("-> Database saved successfully\n");
}

void load_db(void)
{
    FILE *fd = fopen("myteams_db.bin", "rb");
    if (!fd) {
        printf("-> No database found\n");
        server.nb_users = 0;
        server.nb_teams = 0;
        server.nb_channels = 0;
        server.nb_threads = 0;
        return;
    }

    // Load users
    if (fread(&server.nb_users, sizeof(int), 1, fd) == 1 && server.nb_users > 0) {
        fread(server.users, sizeof(user_t), server.nb_users, fd);
        for (int i = 0; i < server.nb_users; ++i) {
            server.users[i].is_connected = false;
            server_event_user_loaded(server.users[i].uuid, server.users[i].name);
        }
    }

    // Load teams
    if (fread(&server.nb_teams, sizeof(int), 1, fd) == 1 && server.nb_teams > 0)
        fread(server.teams, sizeof(team_t), server.nb_teams, fd);

    // Load channels
    if (fread(&server.nb_channels, sizeof(int), 1, fd) == 1 && server.nb_channels > 0)
        fread(server.channels, sizeof(channel_t), server.nb_channels, fd);

    // Load threads
    if (fread(&server.nb_threads, sizeof(int), 1, fd) == 1 && server.nb_threads > 0)
        fread(server.threads, sizeof(thread_t), server.nb_threads, fd);

    // Load messages
    if (fread(&server.nb_messages, sizeof(int), 1, fd) == 1 && server.nb_messages > 0)
        fread(server.messages, sizeof(private_message_t), server.nb_messages, fd);
    
    fclose(fd);
    printf("-> Database loaded successfully\n");
}

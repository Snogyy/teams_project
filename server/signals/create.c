/*
** Project  -  my_teams
** Date     -  April 16th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../server.h"

static int actual_client_fd;
static client_info_t *client;

void send_create_response(char *type, char *payload)
{
    char response[1024];
    snprintf(response, sizeof(response), "251 %s %s", type, payload);
    generate_client_respons(actual_client_fd, response);
}

void send_already_exist_response(void)
{
    generate_client_respons(actual_client_fd, "400 ALREADY_EXIST");
}

bool team_name_exists(char const *name)
{
    for (int i = 0; i < server.nb_teams; i++) {
        if (strcmp(server.teams[i].name, name) == 0)
            return true;
    }
    return false;
}

bool channel_name_exists(char const *team_uuid, char const *name)
{
    for (int i = 0; i < server.nb_channels; i++) {
        if (strcmp(server.channels[i].team_uuid, team_uuid) == 0 && strcmp(server.channels[i].name, name) == 0)
            return true;
    }
    return false;
}

bool thread_title_exists(char const *channel_uuid, char const *title)
{
    for (int i = 0; i < server.nb_threads; i++) {
        if (strcmp(server.threads[i].channel_uuid, channel_uuid) == 0 && strcmp(server.threads[i].title, title) == 0)
            return true;
    }
    return false;
}

// Verify if uuid enter in /use command is valid
bool is_valid_uuid(client_context_t context)
{
    bool team_valid = false;
    bool channel_valid = false;
    bool thread_valid = false;

    if (context.team_uuid[0] != '\0') {
        for (int i = 0; i < server.nb_teams; i++) {
            if (strcmp(server.teams[i].uuid, context.team_uuid) == 0) {
                team_valid = true;
                break;
            }
        }
        if (!team_valid) {
            generate_client_respons(actual_client_fd, "404 TEAM Not Found");
            return false;
        }
    }
    if (context.channel_uuid[0] != '\0') {
        for (int i = 0; i < server.nb_channels; i++) {
            if (strcmp(server.channels[i].uuid, context.channel_uuid) == 0) {
                channel_valid = true;
                break;
            }
        }
        if (!channel_valid) {
            generate_client_respons(actual_client_fd, "404 CHANNEL Not Found");
            return false;
        }
    }
    if (context.thread_uuid[0] != '\0') {
        for (int i = 0; i < server.nb_threads; i++) {
            if (strcmp(server.threads[i].uuid, context.thread_uuid) == 0) {
                thread_valid = true;
                break;
            }
        }
        if (!thread_valid) {
            generate_client_respons(actual_client_fd, "404 THREAD Not Found");
            return false;
        }
    }

    return true;
}


// Create TEAM
void create_team(char **av)
{
    team_t new_team = {
        .subscribers = {0},
        .nb_subscribers = 0
    };

    snprintf(new_team.name, sizeof(new_team.name), "%s", av[0]);
    snprintf(new_team.description, sizeof(new_team.description), "%s", av[1]);

    if (team_name_exists(new_team.name))
        return send_already_exist_response();

    uuid_t id;
    uuid_generate(id);
    uuid_unparse_lower(id, new_team.uuid);
    server.teams[server.nb_teams] = new_team;
    server.nb_teams += 1;

    server_event_team_created(new_team.uuid, new_team.name, client->user_uuid);
    char payload[1024];
    snprintf(payload, sizeof(payload), "\"%s\" \"%s\" \"%s\"", new_team.uuid, new_team.name, new_team.description);
    send_create_response("TEAM", payload);
    snprintf(payload, sizeof(payload), "EVT TEAM_CREATED \"%s\" \"%s\" \"%s\"", new_team.uuid, new_team.name, new_team.description);
    generate_all_logged_clients_event(payload);
}

// Create CHANNEL
void create_channel(char **av)
{
    channel_t new_channel;

    if (!is_valid_uuid(client->context))
        return;

    snprintf(new_channel.name, sizeof(new_channel.name), "%s", av[0]);
    snprintf(new_channel.description, sizeof(new_channel.description), "%s", av[1]);
    snprintf(new_channel.team_uuid, sizeof(new_channel.team_uuid), "%s", client->context.team_uuid);

    if (channel_name_exists(new_channel.team_uuid, new_channel.name))
        return send_already_exist_response();

    uuid_t id;
    uuid_generate(id);
    uuid_unparse_lower(id, new_channel.uuid);
    server.channels[server.nb_channels] = new_channel;
    server.nb_channels += 1;

    server_event_channel_created(new_channel.team_uuid, new_channel.uuid, new_channel.name);
    char payload[1024];
    snprintf(payload, sizeof(payload), "\"%s\" \"%s\" \"%s\"", new_channel.uuid, new_channel.name, new_channel.description);
    send_create_response("CHANNEL", payload);
    snprintf(payload, sizeof(payload), "EVT CHANNEL_CREATED \"%s\" \"%s\" \"%s\"", new_channel.uuid, new_channel.name, new_channel.description);
    generate_team_subscribers_event(new_channel.team_uuid, payload);
}

// Create THREAD
void create_thread(char **av)
{
    thread_t new_thread;

    if (!is_valid_uuid(client->context))
        return;

    snprintf(new_thread.title, sizeof(new_thread.title), "%s", av[0]);
    snprintf(new_thread.body, sizeof(new_thread.body), "%s", av[1]);
    snprintf(new_thread.team_uuid, sizeof(new_thread.team_uuid), "%s", client->context.team_uuid);
    snprintf(new_thread.channel_uuid, sizeof(new_thread.channel_uuid), "%s", client->context.channel_uuid);

    if (thread_title_exists(new_thread.channel_uuid, new_thread.title))
        return send_already_exist_response();

    uuid_t id;
    uuid_generate(id);
    uuid_unparse_lower(id, new_thread.uuid);
    new_thread.timestamp = time(NULL);
    server.threads[server.nb_threads] = new_thread;
    server.nb_threads += 1;

    server_event_thread_created(new_thread.channel_uuid, new_thread.uuid, client->user_uuid, new_thread.title, new_thread.body);
    char payload[1024];
    snprintf(payload, sizeof(payload), "\"%s\" \"%s\" %ld \"%s\" \"%s\"", new_thread.uuid, client->user_uuid, (long)new_thread.timestamp, new_thread.title, new_thread.body);
    send_create_response("THREAD", payload);
    snprintf(payload, sizeof(payload), "EVT THREAD_CREATED \"%s\" \"%s\" %ld \"%s\" \"%s\"", new_thread.uuid, client->user_uuid, (long)new_thread.timestamp, new_thread.title, new_thread.body);
    generate_team_subscribers_event(new_thread.team_uuid, payload);
}

// Create REPLY
void create_reply(char **av)
{
    reply_thread_t new_reply;

    if (!is_valid_uuid(client->context))
        return;

    snprintf(new_reply.body, sizeof(new_reply.body), "%s", av[0]);
    snprintf(new_reply.team_uuid, sizeof(new_reply.team_uuid), "%s", client->context.team_uuid);
    snprintf(new_reply.channel_uuid, sizeof(new_reply.channel_uuid), "%s", client->context.channel_uuid);
    snprintf(new_reply.thread_uuid, sizeof(new_reply.thread_uuid), "%s", client->context.thread_uuid);

    uuid_t id;
    uuid_generate(id);
    uuid_unparse_lower(id, new_reply.uuid);
    new_reply.timestamp = time(NULL);
    server.replies[server.nb_replies] = new_reply;
    server.nb_replies += 1;

    server_event_reply_created(new_reply.thread_uuid, client->user_uuid, new_reply.body);
    char payload[1024];
    snprintf(payload, sizeof(payload), "\"%s\" \"%s\" %ld \"%s\"", new_reply.thread_uuid, client->user_uuid, (long)new_reply.timestamp, new_reply.body);
    send_create_response("REPLY", payload);
    snprintf(payload, sizeof(payload), "EVT REPLY_CREATED \"%s\" \"%s\" %ld \"%s\"", new_reply.thread_uuid, client->user_uuid, (long)new_reply.timestamp, new_reply.body);
    generate_team_subscribers_event(new_reply.team_uuid, payload);
}

void signal_create(char *argument, int client_index)
{
    actual_client_fd = server.pfd_list[client_index].fd;
    client = &server.clients[client_index - 1];
    char *av[2];
    int ac = extract_args(argument, av, 2);

    if (ac < 0)
        generate_client_respons(actual_client_fd, find_reply_server(400));
    else if (ac == 2) {
        if (client->context.context_level == GLOBAL)
            create_team(av);
        else if (client->context.context_level == TEAM)
            create_channel(av);
        else if (client->context.context_level == CHANNEL)
            create_thread(av);
        else
            generate_client_respons(actual_client_fd, find_reply_server(400));
    } else if (ac == 1 && client->context.context_level == THREAD)
        create_reply(av);
    else
        generate_client_respons(actual_client_fd, find_reply_server(400));
}

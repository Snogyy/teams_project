/*
** Project  -  my_teams
** Date     -  April 14th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

#include "../client.h"

int cmd_help(char **args, int client_socket)
{
    printf("Available commands:\n");
    printf("  /help                                                 : Show help\n");
    printf("  /login [\"username\"]                                   : Log in to the server\n");
    printf("  /logout                                               : Disconnect from the server\n");
    printf("  /users                                                : List all users\n");
    printf("  /user [\"user_uuid\"]                                   : Show details of a specific user\n");
    printf("  /send [\"user_uuid\"] [\"message_body\"]                  : Send a private message to a user\n");
    printf("  /messages [\"user_uuid\"]                               : List conversation messages with a user\n");
    printf("  /subscribe [\"team_uuid\"]                              : Subscribe to a team\n");
    printf("  /subscribed ?[\"team_uuid\"]                            : List subscribed teams or users subscribed to a team\n");
    printf("  /unsubscribe [\"team_uuid\"]                            : Unsubscribe from a team\n");
    printf("  /use ?[\"team_uuid\"] ?[\"channel_uuid\"] ?[\"thread_uuid\"] : Set the current context\n");
    printf("  /create                                               : Create an item in the current context (depends on /use)\n");
    printf("  /list                                                 : List items in the current context (depends on /use)\n");
    printf("  /info                                                 : Display info about the current context (depends on /use)\n");

    return 1;
}

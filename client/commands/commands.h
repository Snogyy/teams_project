/*
** Project  -  my_teams
** Date     -  April 13th 2026
**
** Copyright (c) 2026 Jules Nourdin
*/

int cmd_login(char **args, int client_socket);
int cmd_logout(char **args, int client_socket);
int cmd_help(char **args, int client_socket);
int cmd_subscribe(char **args, int client_socket);
int cmd_unsubscribe(char **args, int client_socket);
int cmd_subscribed(char **args, int client_socket);
int cmd_users(char **args, int client_socket);
int cmd_user(char **args, int client_socket);
int cmd_send(char **args, int client_socket);
int cmd_messages(char **args, int client_socket);
int cmd_use(char **args, int client_socket);
int cmd_create(char **args, int client_socket);
int cmd_list(char **args, int client_socket);
int cmd_info(char **args, int client_socket);

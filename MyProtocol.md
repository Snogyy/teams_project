# My Teams Protocol (RFC Like)

This document defines the custom protocol used for communication between the `myteams_cli` (client) and `myteams_server` (server) over TCP sockets.


## Client-to-Server Commands

The following commands are sent from the client to the server:

### Connection & Authentication
            HELP <CRLF>                           : List available commands
            LOGI <SP> "<username>" <CRLF>         : Log in to the server as user
            LOGO <CRLF>                           : Disconnect from the server

### Users & Messaging
            USRS <CRLF>                           : List all users in the database
            USER <SP> "<user_uuid>" <CRLF>        : Get details about a specific user
            SEND <SP> "<user_uuid>" <SP> "<body>" <CRLF> : Send a private message to a user
            MSGS <SP> "<user_uuid>" <CRLF>        : List conversation messages with a user

### Teams & Subscriptions
            SUB  <SP> "<team_uuid>" <CRLF>        : Subscribe to a team
            USUB <SP> "<team_uuid>" <CRLF>        : Unsubscribe from a team
            SUBL <CRLF>                           : List all teams the user is subscribed to
            SUBU <SP> "<team_uuid>" <CRLF>        : List all users subscribed to a team

### Context Management
            USE  <CRLF>                           : Reset context to global
            USE  <SP> "<team_uuid>" [<SP> "<channel_uuid>"] [<SP> "<thread_uuid>"] <CRLF> : Set the current context

### Context-Aware Actions (Depends on USE context)
            INFO <CRLF>                           : Display info about the current context
            LIST <CRLF>                           : List items in the current context (teams/channels/threads/replies)
            CREA <SP> "<arg1>" [<SP> "<arg2>"] <CRLF> : Create an item in the current context


## Server-to-Client Replies

The following respons are sent from the server to the client:

#### 1xx - Informational
         120 Service ready in nnn minutes.
         150 File status okay; about to open data connection.

#### 2xx - Success
         200 Command okay / Context successfully updated.
         214 Help message (Shows available commands to the client).
         220 Service ready for new user.
         221 Service closing control connection (Logged out).
         230 User logged in, proceed.
         240 List/Info payload follows (Used before sending USRS, SUBL, LIST payloads).
         250 Requested action okay, completed (e.g., Message Sent, Valid Subscription).
         251 Requested item created successfully (e.g., Team, Channel, Thread, Reply).

#### 3xx - Pending
         300 Pending further action.

#### 4xx - Client Error
         400 Bad Request (Missing quotes, invalid syntax, missing args).
         401 Unauthorized (Client is not logged in).
         403 Forbidden (User is logged in but lacks rights, e.g. not subscribed to a team).
         404 Not Found (Requested UUID for user, team, channel, or thread does not exist).
         413 Payload Too Large (e.g., name > 32 chars, desc > 255 chars, body > 512 chars).

#### 5xx - Server Error
         500 Syntax error, command unrecognized.
         501 Syntax error in parameters or arguments.
         503 Bad sequence of commands.
         550 Internal server error / Database failure.

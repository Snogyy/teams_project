# My Teams

My Teams is a network programming project inspired by Microsoft Teams.
It provides a TCP server and a CLI client that communicate through a custom line-based protocol.

The project focuses on:
- socket programming in C
- event-driven server architecture with poll()
- custom application protocol design (RFC-style)
- command parsing and context-aware behavior
- state persistence across server restarts

## Project Goal

The goal is to build a collaborative messaging platform where users can:
- log in and out
- list users and inspect user details
- send and read private messages
- subscribe and unsubscribe to teams
- navigate hierarchical contexts (team, channel, thread)
- create and list entities depending on the active context

## Technical Overview

## Client / Server split

Server (myteams_server):
- listens on a TCP port
- accepts multiple clients
- handles requests and sends replies/events
- stores users, teams, channels, threads, replies, and private messages
- saves database state on shutdown and reloads it on startup

Client (myteams_cli):
- connects to the server with IP and port
- provides an interactive CLI
- parses local commands and sends protocol messages
- handles synchronous replies and asynchronous events

## Concurrency model

The server uses poll() for multiplexing.
No thread/fork model is used for request handling.

## Data persistence

Server state is persisted in a local binary file (myteams_db.bin).
The server saves data on SIGINT (Ctrl+C) and restores it on startup.

## Protocol Summary

The full protocol is documented in MyProtocol.md.

## Build

From the project root:

make

This generates:
- myteams_server
- myteams_cli

Clean targets:
- make clean
- make fclean
- make re

## Runtime requirements

The binaries are linked with:
- libmyteams (provided in libs/myteams)
- libuuid

If needed on your machine, export the runtime library path before launching:

export LD_LIBRARY_PATH=libs/myteams:$LD_LIBRARY_PATH

## Run

Start server:

./myteams_server [port]

Start client:

./myteams_cli [ip] [port]

Help:
- ./myteams_server --help
- ./myteams_cli --help

## CLI usage

In the client prompt, commands are slash-prefixed.
String arguments must be enclosed in double quotes.

Core commands:
- /help
- /login "username"
- /logout
- /users
- /user "user_uuid"
- /send "user_uuid" "message_body"
- /messages "user_uuid"
- /subscribe "team_uuid"
- /subscribed ["team_uuid"]
- /unsubscribe "team_uuid"
- /use ["team_uuid"] ["channel_uuid"] ["thread_uuid"]
- /create ...
- /list
- /info

## Context-aware behavior

The commands /create, /list, and /info depend on the active /use context:

No context:
- /create creates a team
- /list lists teams
- /info shows current user info

Team context:
- /create creates a channel
- /list lists channels
- /info shows team info

Team + channel context:
- /create creates a thread
- /list lists threads
- /info shows channel info

Team + channel + thread context:
- /create creates a reply/comment
- /list lists replies
- /info shows thread info

## Notes and limitations

- Before login, protected commands are rejected.
- Commands with malformed quoting are rejected by the client parser.
- Input can be fragmented in the client and is assembled until a full command line is complete.
- Some features may still need additional validation tests depending on your grading workflow.

## Repository layout

- client/: CLI client implementation and command handlers
- server/: server core, request parser, signal handlers
- server/signals/: protocol command handlers on server side
- client/commands/: command handlers on client side
- libs/myteams/: provided logging headers/library
- MyProtocol.md: protocol specification
- Makefile: build rules

## Repository layout

- client/: CLI client implementation and command handlers
- server/: server core, request parser, signal handlers
- server/signals/: protocol command handlers on server side
- client/commands/: command handlers on client side
- libs/myteams/: provided logging headers/library
- MyProtocol.md: protocol specification
- Makefile: build rules

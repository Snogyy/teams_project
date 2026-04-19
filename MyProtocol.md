# My Teams Wire Protocol Specification

## 1. Scope
This document specifies the application-layer protocol used between
`myteams_cli` and `myteams_server` over TCP.

This specification is intentionally aligned with the current implementation.

## 2. Conformance Language
The key words MUST, MUST NOT, REQUIRED, SHOULD, SHOULD NOT, and MAY are to be
interpreted as described in RFC 2119.

## 3. Transport and Framing
- Transport is TCP.
- Each protocol message MUST be terminated by `CRLF` (`\r\n`).
- The protocol is line-oriented.

## 4. Authentication Rule
Before login, the server accepts only `LOGI`.
Any other command MUST be rejected with `401 Unauthorized.`.

## 5. Client-to-Server Commands

### 5.1 Authentication
- `LOGI "<username>"`
- `LOGO`

### 5.2 Users and Messaging
- `USRS`
- `USER "<user_uuid>"`
- `SEND "<user_uuid>" "<message_body>"`
- `MSGS "<user_uuid>"`

### 5.3 Subscriptions
- `SUB "<team_uuid>"`
- `USUB "<team_uuid>"`
- `SUBL`
- `SUBU "<team_uuid>"`

### 5.4 Context
- `USE`
- `USE "<team_uuid>"`
- `USE "<team_uuid>" "<channel_uuid>"`
- `USE "<team_uuid>" "<channel_uuid>" "<thread_uuid>"`

### 5.5 Context-Aware Operations
- `CREA "<arg1>"`
- `CREA "<arg1>" "<arg2>"`
- `LIST`
- `INFO`

Note: `/help` is local CLI behavior and does not send a wire command.

## 6. Server-to-Client Replies

### 6.1 Generic Status Replies
- `220 Service ready for new user.`
- `230 <user_uuid> <username>`
- `240 List/Info payload follows.`
- `250 Requested action okay, completed.`
- `251 Requested item created successfully.`
- `400 Bad Request.`
- `401 Unauthorized.`
- `403 Forbidden.`
- `404 Not Found.`
- `413 Payload Too Large.`
- `500 Syntax error, command unrecognized.`
- `550 Internal server error / Database failure.`
- `560 Server full.`

### 6.2 Contextual and Extended Replies (Implemented)
- `400 ALREADY_EXIST`
- `404 TEAM Not Found`
- `404 CHANNEL Not Found`
- `404 THREAD Not Found`
- `242 <user_uuid> <team_uuid>`
- `243 <team_uuid> "<team_name>" "<team_description>"`
- `244 <user_uuid> "<user_name>" <0|1>`
- `431 Already subscribed`
- `432 Not subscribed to this team`
- `440 <team_uuid>`

### 6.3 Data Lines
- `USER <user_uuid> <user_name> <0|1>`
- `MSG <sender_uuid> <timestamp> "<body>"`
- `USE GLOBAL`
- `USE TEAM`
- `USE CHANNEL`
- `USE THREAD`

## 7. Typed Payload Format (`251`)
The implementation uses typed payload lines for create/list/info:

- `251 TEAM "<team_uuid>" "<team_name>" "<team_description>"`
- `251 CHANNEL "<channel_uuid>" "<channel_name>" "<channel_description>"`
- `251 THREAD "<thread_uuid>" "<user_uuid>" <timestamp> "<thread_title>" "<thread_body>"`
- `251 REPLY "<thread_uuid>" "<user_uuid>" <timestamp> "<reply_body>"`
- `251 USER "<user_uuid>" "<user_name>" <0|1>`

## 8. Asynchronous Events (`EVT`)
Events are sent independently of command-response flow.

- `EVT LOGIN "<user_uuid>" "<user_name>"`
- `EVT LOGOUT "<user_uuid>" "<user_name>"`
- `EVT PRIVATE_MESSAGE "<sender_uuid>" "<message_body>"`
- `EVT TEAM_CREATED "<team_uuid>" "<team_name>" "<team_description>"`
- `EVT CHANNEL_CREATED "<channel_uuid>" "<channel_name>" "<channel_description>"`
- `EVT THREAD_CREATED "<thread_uuid>" "<user_uuid>" <timestamp> "<thread_title>" "<thread_body>"`
- `EVT REPLY_CREATED "<thread_uuid>" "<user_uuid>" <timestamp> "<reply_body>"`

Event delivery scope in current implementation:
- LOGIN/LOGOUT: logged clients.
- TEAM_CREATED: all logged clients.
- CHANNEL_CREATED/THREAD_CREATED/REPLY_CREATED: logged subscribers of the team.
- PRIVATE_MESSAGE: all logged sessions of the target user.

## 9. Context Semantics for `INFO`
- Global context (`USE`): `INFO` returns current logged user details.
- Team context (`USE "team_uuid"`): `INFO` returns selected team details.
- Channel context (`USE "team_uuid" "channel_uuid"`): `INFO` returns selected channel details.
- Thread context (`USE "team_uuid" "channel_uuid" "thread_uuid"`): `INFO` returns selected thread details.

## 10. Context Semantics for `LIST`
- Global: list teams.
- Team context: list channels of selected team.
- Channel context: list threads of selected channel.
- Thread context: list replies of selected thread.

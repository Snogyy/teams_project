##
## Project  -  my_teams
## Date     -  April 14th 2026
##
## Copyright (c) 2026 Jules Nourdin
##

SRC_SERVER 	=	server/server.c \
				server/handle_client.c \
				server/utils.c \
				server/parse_signals.c \
				server/db.c \
				server/signals/login.c \
				server/signals/logout.c \
				server/commands/users.c \
				server/signals/user.c \
				server/signals/send.c \
				server/signals/messages.c \
				server/signals/subscribe.c \
				server/signals/unsubscribe.c

SRC_CLIENT 	=	client/client.c \
				client/utils.c \
				client/parse_commands.c \
				client/commands/login.c \
				client/commands/logout.c \
				client/commands/help.c \
				client/commands/users.c \
				client/commands/user.c \
				client/commands/send.c \
				client/commands/messages.c \
				client/commands/subscribe.c \
				client/commands/unsubscribe.c

OBJ_SERVER 	=	$(SRC_SERVER:.c=.o)
OBJ_CLIENT 	=	$(SRC_CLIENT:.c=.o)

FLAGS	+=	-Llibs/myteams -lmyteams -luuid
# export LD_LIBRARY_PATH=libs/myteams:$LD_LIBRARY_PATH
# Definir le chemin pour trouver la lib en var system

BIN_SERVER 	= 	myteams_server
BIN_CLIENT 	= 	myteams_cli


all:	server client

server:	$(OBJ_SERVER)
	gcc -o $(BIN_SERVER) $(OBJ_SERVER) $(FLAGS)

client:	$(OBJ_CLIENT)
	gcc -o $(BIN_CLIENT) $(OBJ_CLIENT) $(FLAGS)

clean :
	rm -f $(OBJ_SERVER)
	rm -f $(OBJ_CLIENT)

fclean: clean
	rm -f $(BIN_SERVER)
	rm -f $(BIN_CLIENT)

re: fclean all

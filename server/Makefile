## ----------------------------------------------------------------------------------- ##
##                                                                                     ##
## EPITECH PROJECT - Tue, Feb, 2026                                                    ##
## Title           - bs-ftp                                                            ##
## Description     -                                                                   ##
##     Makefile                                                                        ##
##                                                                                     ##
## ----------------------------------------------------------------------------------- ##
##                                                                                     ##
##         ░        ░       ░░        ░        ░        ░░      ░░  ░░░░  ░            ##
##         ▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒  ▒▒▒▒  ▒            ##
##         ▓      ▓▓▓       ▓▓▓▓▓  ▓▓▓▓▓▓▓  ▓▓▓▓      ▓▓▓  ▓▓▓▓▓▓▓        ▓            ##
##         █  ███████  ██████████  ███████  ████  ███████  ████  █  ████  █            ##
##         █        █  ███████        ████  ████        ██      ██  ████  █            ##
##                                                                                     ##
## ----------------------------------------------------------------------------------- ##

SRC 	=	server.c \
			handle_client.c \
			utils.c \
			parse_signals.c \
			db.c \
			signals/user.c \
			signals/quit.c

OBJ 	=	$(SRC:.c=.o)

FLAGS	+=	-L../libs/myteams -lmyteams -Wl,-rpath=../libs/myteams -luuid

NAME 	= 	myteams_server

all:	$(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) $(FLAGS)

clean :
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

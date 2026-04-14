##
## Project  -  my_teams
## Date     -  March 30th 2026
##
## Copyright (c) 2026 Jules Nourdin
##

SRC 	=	client.c \
			utils.c \
			parse_commands.c \
			commands/login.c

OBJ 	=	$(SRC:.c=.o)

FLAGS	+=	-L../libs/myteams -lmyteams -Wl,-rpath=../libs/myteams -luuid

NAME 	= 	myteams_cli

all:	$(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) $(FLAGS)

clean :
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

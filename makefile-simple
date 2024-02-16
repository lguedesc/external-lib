CC=clang
STD=c17
NAME=main
LIBS=odesolvers.c
DLIB=duffing
all:
	@$(CC) -std=$(STD) -o $(NAME) $(LIBS) main.c -ldl

run: all
	@./$(NAME)

dlib:
	@$(CC) -shared -o $(DLIB).dylib $(DLIB).c	

clean:
	@rm $(NAME)
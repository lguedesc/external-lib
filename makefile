CC=clang
STD=c17
NAME=main
LIBS=odesolvers.c
DLIB=duffing

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    SHARED_LIB_EXT := .so
endif
ifeq ($(UNAME_S),Darwin)
    SHARED_LIB_EXT := .dylib
endif
ifeq ($(UNAME_S),Windows_NT)
    SHARED_LIB_EXT := .dll
endif

all: dlib
	@$(CC) -std=$(STD) -o $(NAME) $(LIBS) main.c -ldl

run: all
	@./$(NAME)

dlib:
	@$(CC) -shared -o $(DLIB)$(SHARED_LIB_EXT) $(DLIB).c	

clean:
	@rm $(NAME)

clean-dlib:
	@rm $(DLIB)$(SHARED_LIB_EXT)

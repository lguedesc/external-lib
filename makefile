#"C:\Program Files (x86)\Intel\oneAPI\setvars.bat" 

CC=gcc
STD=c17
NAME=main
LIBS=odesolvers.c
DLIB=duffing

# Determine platform and set parameters accordingly
ifeq ($(OS),Windows_NT)
	SHARED_LIB_FLAG =
    SHARED_LIB_EXT := .dll
else
	SHARED_LIB_FLAG = -ldl
    UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
        SHARED_LIB_EXT := .so
    endif
    ifeq ($(UNAME_S),Darwin)
        SHARED_LIB_EXT := .dylib
    endif
	
endif


all: dlib
	$(CC) -std=$(STD) -o $(NAME) $(LIBS) main.c $(SHARED_LIB_FLAG)

run: all
	@./$(NAME)

dlib:
	$(CC) -shared -o $(DLIB)$(SHARED_LIB_EXT) $(DLIB).c	

clean:
	@rm $(NAME)

clean-dlib:
	@rm $(DLIB)$(SHARED_LIB_EXT)

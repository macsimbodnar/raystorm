# Get the operating system
UNAME := $(shell uname)

APP_NAME = raystorm
# -Wno-write-strings -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-sign-compare -Wno-unused-result -Wno-strict-aliasing -Wno-switch -fno-exceptions
RAYSTORM_FLAGS = -DRS_DEBUG
DISABLE_WARNINGS =
DEBUG_CFLAGS = -O0 -g -fsanitize=undefined,address -ftrapv 

CFLAGS = -Wall -Wextra -pedantic -Werror $(RAYSTORM_FLAGS) $(DISABLE_WARNINGS) $(DEBUG_CFLAGS)
LIBS =

# In case we compile on macos (Darwin is the name) we needs to set the openssl dir
ifeq ($(UNAME), Darwin)
LIBS += -l SDL2-2.0.0
else
LIBS += -lSDL2 -lm
endif

OBJS = common_platform.o intrinsics.o world.o renderer.o raystorm.o

all: $(APP_NAME)

run: $(APP_NAME)
	./$(APP_NAME)

%.o : %.c %.h log.h assets.h
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_NAME): sdl_platform.c $(OBJS)
	$(CC) $(CFLAGS) sdl_platform.c -o $(APP_NAME) $(OBJS) $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o $(APP_NAME)
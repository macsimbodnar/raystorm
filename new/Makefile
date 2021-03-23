# Get the operating system
UNAME := $(shell uname)

APP_NAME = raystorm
CFLAGS = -O0
LIBS =

# In case we compile on macos (Darwin is the name) we needs to set the openssl dir
ifeq ($(UNAME), Darwin)
LIBS += -framework GLUT -framework OpenGL -framework Cocoa
else
LIBS += -lGL -lGLU -lglut
endif

OBJS = renderer.o raystorm.o

all: $(APP_NAME)

run: $(APP_NAME)
	./$(APP_NAME)

%.o : %.c %.h log.h
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_NAME): platform.c $(OBJS)
	$(CC) $(CFLAGS) platform.c -o $(APP_NAME) $(OBJS) $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o $(APP_NAME)
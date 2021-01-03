# Get the operating system
UNAME := $(shell uname)

APP_NAME = raystorm
FLAGS = -O0
LIBS =

# In case we compile on macos (Darwin is the name) we needs to set the openssl dir
ifeq ($(UNAME), Darwin)
LIBS += -framework GLUT -framework OpenGL -framework Cocoa
else
LIBS += -lGL -lGLU -lglut
endif

all: $(APP_NAME)

$(APP_NAME): raystorm.cpp
	$(CXX) $(FLAGS) raystorm.cpp -o $(APP_NAME) $(LIBS)

raystorm_sdl: raystorm_sdl.cpp 
	$(CXX) $(FLAGS) -I./renderer raystorm_sdl.cpp renderer/pixello.o -o raystorm_sdl -lSDL2

.PHONY: clean
clean:
	rm -rf *.o $(APP_NAME) raystorm_sdl
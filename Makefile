APP_NAME = raystorm
LIBS = -lGL -lGLU -lglut
FLAGS = -O0

all: $(APP_NAME)

$(APP_NAME): raystorm.cpp
	$(CXX) $(FLAGS) raystorm.cpp -o $(APP_NAME) $(LIBS)

raystorm_sdl: raystorm_sdl.cpp 
	$(CXX) $(FLAGS) -I./renderer raystorm_sdl.cpp renderer/pixello.o -o raystorm_sdl -lSDL2

.PHONY: clean
clean:
	rm -rf *.o $(APP_NAME) raystorm_sdl
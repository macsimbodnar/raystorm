APP_NAME = raystorm
LIBS = -lGL -lGLU -lglut
FLAGS = -O0

all: $(APP_NAME)

$(APP_NAME): raystorm.cpp
	$(CXX) $(FLAGS) raystorm.cpp -o $(APP_NAME) $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o $(APP_NAME)
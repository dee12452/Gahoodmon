CC = g++
FILES = ../src/Main.cpp $(wildcard ../src/game/*.cpp) $(wildcard ../src/map/*cpp) $(wildcard ../src/screen/*.cpp) $(wildcard ../src/util/*.cpp) $(wildcard ../src/sprite/*.cpp) $(wildcard ../src/world/*.cpp)
FILES_NOMAP = ../src/Main.cpp $(wildcard ../src/game/*.cpp) $(wildcard ../src/screen/*.cpp) $(wildcard ../src/util/*.cpp)
FLAGS = -std=c++11
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf
OUT = game.out
game:
	$(CC) $(FILES) -o $(OUT) $(FLAGS) $(LIBS)
nomap:
	$(CC) $(FILES_NOMAP) -o $(OUT) $(FLAGS) $(LIBS)

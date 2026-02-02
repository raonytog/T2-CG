# the compiler: gcc for C program, define as g++ for C++
CC = gcc
CXX = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
LINKING = -lglut -lGL -lGLU
TARGET = *

ARENA = arenas/arena.svg

all: src/*.cpp includes/*.h
	$(CXX) -o trabalhocg src/$(TARGET).cpp $(LINKING)

run:
	make
	./trabalhocg ARENA

clean:
	rm -f trabalhocg trabalho_valgrind valgrind_out.txt

valgrind: src/*.cpp includes/*.h
	$(CXX) $(CFLAGS) -O0 -o trabalho_valgrind src/$(TARGET).cpp $(LINKING)

run_valgrind_%: trabalho_valgrind
	valgrind --leak-check=full ./trabalho_valgrind $* 2> valgrind_out.txt

run_valgrind_all_%: trabalho_valgrind
	valgrind --leak-check=full --show-leak-kinds=all ./trabalho_valgrind $* 2> valgrind_out.txt
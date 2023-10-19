SRC = main.cpp server.cpp
FLAGS = -g -Wall -std=c++20

all: build

build: 
	mkdir -p bin
	g++ $(FLAGS) $(SRC) -o bin/webserver

clean:
	rm -rf bin 

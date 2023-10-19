SRC = main.cpp server.cpp
FLAGS = -g -Wall

all: build

build: 
	mkdir -p bin
	g++ $(FLAGS) $(SRC) -o bin/webserver

clean:
	rm bin -r

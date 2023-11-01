SRC = main.cpp Proxy/Proxy.cpp Proxy/ProxyCLI.cpp
FLAGS = -g -Wall -std=c++20

all: build

build: 
	mkdir -p bin
	g++ $(FLAGS) $(SRC) -o bin/proxy

clean:
	rm -rf bin 

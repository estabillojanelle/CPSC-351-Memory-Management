all: main process

main: main.cpp
	g++ -o main main.cpp

process: process.cpp
	g++ -o process process.cpp

all: main 

main: main.cpp process.cpp
	g++ -o main.cpp process.cpp main


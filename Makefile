all: build

build:
	g++ -std=c++11 main.cpp -lcurl -lboost_regex -lboost_program_options -laws-cpp-sdk-sts -laws-cpp-sdk-core

run: build
	./a.out

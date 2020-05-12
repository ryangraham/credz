all: build
	./a.out

build:
	g++ -std=c++11 main.cpp -lcurl -lboost_regex -laws-cpp-sdk-sts -laws-cpp-sdk-core

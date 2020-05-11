all: build
	./a.out

build:
	g++ -std=c++11 main.cpp -lcurl -lz -lboost_regex

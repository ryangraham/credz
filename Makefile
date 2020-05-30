all:
	@echo "build - build"
	@echo "clean - remove built files"
	@echo "docker - build docker CI base image locally"
	@echo "install - install to /usr/local/bin/"
	@echo "pretty - run clang-format"
	@echo "push - Push docker CI base image to dockerhub"
	@echo "run - build and run"
	@echo "test - run doctests and exit"
	@echo "tidy - run clang-tidy"

.PHONY: build
build:
	cd build && cmake .. && $(MAKE)

clean:
	rm -rf ./build/* || true

docker:
	docker build -t ryangraham/credz:0.0.5 .

install: clean build
	cd build && $(MAKE) install

pretty:
	clang-format -i --style=file src/*.cc src/*.h

push:
	docker push ryangraham/credz:0.0.5

run: build
	./build/credz

test: build
	./build/credz -t

tidy:
	clang-tidy src/* -p ./build

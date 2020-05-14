all:
	@echo "build - build"
	@echo "clean - remove built files"
	@echo "run - build and run"

.PHONY: build
build:
	cd build && cmake .. && $(MAKE)

clean:
	rm -rf ./build/* || true

run: build
	./build/credz

docker:
	docker build -t ryangraham/credz:0.0.1 .

push:
	docker push ryangraham/credz:0.0.1

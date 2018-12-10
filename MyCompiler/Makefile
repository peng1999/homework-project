.PHONY: all clean cmake

NAME=lang

all: cmake

cmake:
	cd cmake-build-debug && { cmake .. && make; cd ..; }

clean:
	cd cmake-build-debug && { make clean; cd ..; }


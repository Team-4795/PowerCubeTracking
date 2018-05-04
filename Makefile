SRC_FILES = $(wildcard src/*.cpp)
BUILD_FILES = $(patsubst src/%.cpp, build/%.o, ${SRC_FILES})
LIBS = opencv
LFLAGS = $(shell pkg-config --libs ${LIBS})
CFLAGS = -std=gnu++11 -g $(shell pkg-config --cflags ${LIBS})

all: build ${BUILD_FILES}
	g++ -o build/PowerCubeTracking ${BUILD_FILES} ${LFLAGS} -lntcore
clean:
	-rm -rf build/
build/%.o: src/%.cpp
	g++ ${CFLAGS} -c -o $@ $^
build:
	mkdir build

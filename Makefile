DEPS=sdl2 freetype2 glew

TARGET_NAME=te

CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -g `pkg-config --cflags $(DEPS)`
LIBS=-lm `pkg-config --libs $(DEPS)`

SRCS = $(wildcard src/*.c src/editor/*.c)
HDRS = $(wildcard src/*.h src/editor/*.h)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

$(TARGET_NAME): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS) -lGL -lGLU

build/%.o: src/%.c | build/editor
	$(CC) $(CFLAGS) $< -c -o $@

build/editor:
	mkdir -p build/editor

all: te

run:
	./te &

clean:
	rm ./te
	rm -r build/

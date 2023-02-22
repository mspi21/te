DEPS=sdl2 freetype2 glew

TARGET_NAME=te

CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -g `pkg-config --cflags $(DEPS)`
LIBS=-lm `pkg-config --libs $(DEPS)`

SRCS = $(wildcard src/*.c)
HDRS = $(wildcard src/*.h)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

$(TARGET_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS) -lGL -lGLU

build/%.o: src/%.c | build/
	$(CC) $(CFLAGS) $< -c -o $@

build/:
	mkdir -p build

all: te

run:
	./te

clean:
	rm ./te

Makefile.d:
	$(CC) -MM $(SRCS) > Makefile.d

include Makefile.d

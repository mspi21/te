DEPS=sdl2 freetype2 glew

CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -g `pkg-config --cflags $(DEPS)`
LIBS=-lm `pkg-config --libs $(DEPS)`

HEADER_FILES=src/file.h src/font.h src/renderer.h src/vec.h src/editor.h src/dialog.h src/cmd_parser.h src/utils.h
IMPL_FILES=src/file.c src/font.c src/main.c src/renderer.c src/vec.c src/editor.c src/dialog.c src/cmd_parser.c src/utils.c

te: $(HEADER_FILES) $(IMPL_FILES)
	$(CC) $(CFLAGS) -o te $(IMPL_FILES) $(LIBS) -lGL -lGLU

all: te

clean:
	rm ./te
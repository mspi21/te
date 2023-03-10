#ifndef CURSOR_H_
#define CURSOR_H_

#include "line.h"

#include <stdbool.h>

typedef struct {
    size_t row;
    size_t col;

    size_t col_persist;
} Cursor;

void cursor_init(Cursor *cursor);

void cursor_clamp(Cursor *cursor, LineBuffer *lb);

void cursor_set(Cursor *cursor, LineBuffer *lb, size_t row, size_t col);

void cursor_advance(Cursor *cursor, LineBuffer *lb, size_t n);

bool cursor_move_left(Cursor *cursor, LineBuffer *lb);

bool cursor_move_right(Cursor *cursor, LineBuffer *lb);

bool cursor_move_up(Cursor *cursor, LineBuffer *lb);

bool cursor_move_down(Cursor *cursor, LineBuffer *lb);

bool cursor_skip_word_left(Cursor *cursor, LineBuffer *lb);

bool cursor_skip_word_right(Cursor *cursor, LineBuffer *lb);

void cursor_destroy(Cursor *cursor);

#endif // CURSOR_H_
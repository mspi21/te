#ifndef CURSOR_H_
#define CURSOR_H_

#include "line.h"

#include <stdbool.h>

typedef struct {
    size_t row;
    size_t col;
} Cursor;

void cursor_clamp(Cursor *cursor, LineBuffer *lb);

void cursor_set(Cursor *cursor, LineBuffer *lb, size_t row, size_t col);

bool cursor_move_left(Cursor *cursor, LineBuffer *lb);

bool cursor_move_right(Cursor *cursor, LineBuffer *lb);

bool cursor_move_up(Cursor *cursor, LineBuffer *lb);

bool cursor_move_down(Cursor *cursor, LineBuffer *lb);

bool cursor_skip_word_left(Cursor *cursor, LineBuffer *lb);

bool cursor_skip_word_right(Cursor *cursor, LineBuffer *lb);

#endif // CURSOR_H_
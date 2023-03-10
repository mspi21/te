#include "cursor.h"

#include "../utils.h"

void cursor_init(Cursor *cursor) {
    cursor->col = cursor->row = cursor->col_persist = 0;
}

void cursor_clamp(Cursor *cursor, LineBuffer *lb) {
    if(cursor->row >= lb->lines_size)
        cursor->row = lb->lines_size - 1;
    
    if(cursor->col > lb->lines[cursor->row].buffer_size)
        cursor->col = lb->lines[cursor->row].buffer_size;
}

void cursor_set(Cursor *cursor, LineBuffer *lb, size_t row, size_t col) {
    cursor->row = row;
    cursor->col = cursor->col_persist = col;
    cursor_clamp(cursor, lb);
}

void cursor_advance(Cursor *cursor, LineBuffer *lb, size_t n) {
    while(n > lb->lines[cursor->row].buffer_size - cursor->col) {
        n -= lb->lines[cursor->row].buffer_size - cursor->col;
        ++cursor->row;
        cursor->col = 0;
    }
    cursor->col_persist = (cursor->col += n);
}

bool cursor_move_left(Cursor *cursor, LineBuffer *lb) {
    if(cursor->col)
        return cursor->col_persist = --cursor->col, true;
    if(cursor->row)
        return cursor->col_persist = cursor->col = lb->lines[--cursor->row].buffer_size, true;
    return false;
}

bool cursor_move_right(Cursor *cursor, LineBuffer *lb) {
    if(cursor->col < lb->lines[cursor->row].buffer_size)
        return cursor->col_persist = ++cursor->col, true;
    if(cursor->row < lb->lines_size - 1)
        return ++cursor->row, cursor->col_persist = cursor->col = 0, true;
    return false;
}

bool cursor_move_up(Cursor *cursor, LineBuffer *lb) {
    if(cursor->row) {
        --cursor->row;
        cursor->col = cursor->col_persist;
        cursor_clamp(cursor, lb);
        return true;
    }
    
    bool ret = cursor->col != 0;
    cursor->col_persist = cursor->col = 0;
    return ret;
}

bool cursor_move_down(Cursor *cursor, LineBuffer *lb) {
    if(cursor->row < lb->lines_size - 1) {
        ++cursor->row;
        cursor->col = cursor->col_persist;
        cursor_clamp(cursor, lb);
        return true;
    }
    
    bool ret = cursor->col != lb->lines[cursor->row].buffer_size;
    cursor->col_persist = cursor->col = lb->lines[cursor->row].buffer_size;
    return ret;
}

bool cursor_skip_word_left(Cursor *cursor, LineBuffer *lb) {
    if(!cursor->col)
        return cursor_move_left(cursor, lb);
    
    while(
        cursor->col &&
        utils_is_word_boundary(lb->lines[cursor->row].buffer[cursor->col])
    )
        cursor->col_persist = --cursor->col;
    while(
        cursor->col &&
        !utils_is_word_boundary(lb->lines[cursor->row].buffer[cursor->col])
    )
        cursor->col_persist = --cursor->col;
    return true;
}

bool cursor_skip_word_right(Cursor *cursor, LineBuffer *lb) {
    if(cursor->col == lb->lines[cursor->row].buffer_size)
        return cursor_move_right(cursor, lb);

    while(
        cursor->col < lb->lines[cursor->row].buffer_size &&
        utils_is_word_boundary(lb->lines[cursor->row].buffer[cursor->col])
    )
        cursor->col_persist = ++cursor->col;

    while(
        cursor->col < lb->lines[cursor->row].buffer_size &&
        !utils_is_word_boundary(lb->lines[cursor->row].buffer[cursor->col])
    )
        cursor->col_persist = ++cursor->col;
    return true;
}

void cursor_destroy(Cursor *cursor) {
    (void) cursor;
}

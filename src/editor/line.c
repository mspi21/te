#include "line.h"

#include <memory.h>
#include <assert.h>
#include <stdlib.h>

#include "../utils.h"

/* Symbolic constants */

#define LINE_INITIAL_CAPACITY 64
#define LINE_BUFFER_INITIAL_CAPACITY 32

/* Line methods */

void line_create(Line *line) {
    *line = (Line) {
        .buffer = (char *) malloc(LINE_INITIAL_CAPACITY),
        .buffer_capacity = LINE_INITIAL_CAPACITY,
        .buffer_size = 0
    };
}

void line_create_copy(Line *line, const char *src, size_t src_length) {
    *line = (Line) {
        .buffer = (char *) malloc(src_length),
        .buffer_capacity = src_length,
        .buffer_size = src_length
    };
    memcpy(line->buffer, src, src_length);
}

void line_grow(Line *line) {
    line->buffer_capacity = line->buffer_capacity * 2 + LINE_INITIAL_CAPACITY;
    line->buffer = (char *) realloc(line->buffer, line->buffer_capacity * sizeof(char));
}

void line_insert_text(Line *line, size_t pos, const char *text, size_t text_length) {
    while(line->buffer_size + text_length > line->buffer_capacity)
        line_grow(line);
    
    memmove(
        line->buffer + pos + text_length,
        line->buffer + pos,
        line->buffer_size - pos
    );

    memcpy(
        line->buffer + pos,
        text,
        text_length
    );

    line->buffer_size += text_length;
}

void line_delete_text(Line *line, size_t start, size_t end) {
    if(end <= start || end > line->buffer_size)
        return;

    memmove(
        line->buffer + start,
        line->buffer + end,
        line->buffer_size - end
    );

    line->buffer_size -= end - start;
}

void line_destroy(Line *line) {
    free(line->buffer);
    line->buffer = NULL;
}

/* LinesBuffer methods */

void lines_create(LineBuffer *lb) {
    *lb = (LineBuffer) {
        .lines = (Line *) malloc(LINE_BUFFER_INITIAL_CAPACITY * sizeof(Line)),
        .lines_capacity = LINE_BUFFER_INITIAL_CAPACITY,
        .lines_size = 0
    };
}

void lines_grow(LineBuffer *lb) {
    lb->lines_capacity = lb->lines_capacity * 2 + LINE_BUFFER_INITIAL_CAPACITY;
    lb->lines = (Line *) realloc(lb->lines, lb->lines_capacity * sizeof(Line));
}

void lines_grow_if_full(LineBuffer *lb) {
    if(lb->lines_size == lb->lines_capacity)
        lines_grow(lb);
}

void lines_swap(LineBuffer *lb, size_t i, size_t j) {
    assert(i < lb->lines_size && j < lb->lines_size);
    Line tmp = lb->lines[i];
    lb->lines[i] = lb->lines[j];
    lb->lines[j] = tmp; 
}

void lines_move_raw(LineBuffer *lb, size_t from, size_t to, size_t n) {
    memmove(
        lb->lines + to,
        lb->lines + from,
        n * sizeof(*lb->lines)
    );
}

void lines_split(LineBuffer *lb, size_t row, size_t col) {
    lines_grow_if_full(lb);

    Line *selected_line = &lb->lines[row];
    
    Line new_line;
    line_create_copy(
        &new_line,
        selected_line->buffer + col,
        selected_line->buffer_size - col
    );

    selected_line->buffer_size = col;
    lines_move_raw(lb, row + 1, row + 2, lb->lines_size - (row + 1));

    lb->lines[row + 1] = new_line;
    ++lb->lines_size;
}

void lines_clear(LineBuffer *lb) {
    for(size_t i = 0; i < lb->lines_size; ++i)
        line_destroy(&lb->lines[i]);
    lb->lines_size = 0;
}

void lines_append_line(LineBuffer *lb, const char *src, size_t src_length) {
    lines_grow_if_full(lb);
    line_create_copy(
        &lb->lines[lb->lines_size++], src, src_length
    );
}

void lines_insert_line(
    LineBuffer *lb, size_t i, const char *src, size_t src_length
) {
    lines_grow_if_full(lb);
    lines_move_raw(lb, i, i + 1, lb->lines_size - i);
    line_create_copy(&lb->lines[i], src, src_length);
}

// FIXME
void lines_insert_at(
    LineBuffer *lb, size_t row, size_t col, const char *src, size_t src_length
) {
    size_t pos = 0, last_pos = 0;
    if((pos = utils_find_next_line(src, pos, src_length)) == src_length) {
        line_insert_text(&lb->lines[row], col, src, src_length);
        return;
    }
    while(pos < src_length && src[pos]) {
        line_insert_text(&lb->lines[row], col, src + last_pos, pos - last_pos);
        
        assert(src[pos] == '\n');
        lines_split(lb, row, col);
        ++row;
        col = 0;
        
        last_pos = pos;
        pos = utils_find_next_line(src, pos, src_length);
    }
    line_insert_text(&lb->lines[row], col, src + last_pos, pos - last_pos);
}

void lines_range_to_str(
    LineBuffer *lb, size_t rs, size_t cs, size_t re, size_t ce,
    char **dest, size_t *dest_length
) {
    assert(re >= rs);
    if(rs == re) {
        *dest_length = ce - cs;
        *dest = (char *) malloc(*dest_length + 1);
        memcpy(*dest, lb->lines[rs].buffer + cs, ce - cs);
        (*dest)[*dest_length] = 0; // null terminator
        return;
    }

    *dest_length = 0;
    *dest_length += lb->lines[rs].buffer_size - cs + 1; // + 1 for \n
    for(size_t i = rs + 1; i < re; ++i)
        *dest_length = lb->lines[i].buffer_size + 1; // + 1 for \n
    *dest_length += ce;

    *dest = (char *) malloc(*dest_length + 1); // + 1 for NT
    size_t buffer_pos = 0;

    memcpy(*dest + buffer_pos, lb->lines[rs].buffer + cs, lb->lines[rs].buffer_size - cs);
    buffer_pos += lb->lines[rs].buffer_size - cs;
    (*dest)[buffer_pos++] = '\n';

    for(size_t i = rs + 1; i < re; ++i) {
        memcpy(*dest + buffer_pos, lb->lines[rs].buffer, lb->lines[rs].buffer_size);
        buffer_pos += lb->lines[rs].buffer_size;
        (*dest)[buffer_pos++] = '\n';
    }

    memcpy(*dest + buffer_pos, lb->lines[re].buffer, ce);
    buffer_pos += ce;
    (*dest)[buffer_pos] = '\n';
    assert(buffer_pos == *dest_length);
}

void lines_delete_range(
    LineBuffer *lb, size_t rs, size_t cs, size_t re, size_t ce
) {
    if(rs == re) {
        line_delete_text(&lb->lines[rs], cs, ce);
        return;
    }

    line_delete_text(&lb->lines[rs], cs, lb->lines[rs].buffer_size);
    line_insert_text(
        &lb->lines[rs],
        lb->lines[rs].buffer_size,
        lb->lines[re].buffer + ce,
        lb->lines[re].buffer_size - ce
    );
    for(size_t i = rs + 1; i <= re; ++i)
        line_destroy(&lb->lines[i]);
    if(re != lb->lines_size - 1)
        lines_move_raw(lb, re + 1, rs + 1, lb->lines_size - (re + 1));
    lb->lines_size -= re - rs;
}

void lines_destroy(LineBuffer *lb) {
    for(size_t i = 0; i < lb->lines_size; ++i)
        line_destroy(&lb->lines[i]);
    free(lb->lines);
    lb->lines = NULL;
    lb->lines_size = 0;
    lb->lines_capacity = 0;
}
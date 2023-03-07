#ifndef LINE_H_
#define LINE_H_

#include <stddef.h>

typedef struct {
    char *buffer;
    size_t buffer_size;
    size_t buffer_capacity;
} Line;

typedef struct {
    Line *lines;
    size_t lines_size;
    size_t lines_capacity;
} LineBuffer;

/* Line methods */

void line_create(Line *line);

void line_create_copy(Line *line, const char *src, size_t src_length);

void line_grow(Line *line);

void line_insert_text(Line *line, size_t pos, const char *text, size_t text_length);

void line_delete_text(Line *line, size_t start, size_t end);

void line_destroy(Line *line);

/* LinesBuffer methods */

void lines_create(LineBuffer *lb);

void lines_grow(LineBuffer *lb);

void lines_grow_if_full(LineBuffer *lb);

void lines_swap(LineBuffer *lb, size_t i, size_t j);

void lines_move_raw(LineBuffer *lb, size_t from, size_t to, size_t n);

void lines_split(LineBuffer *lb, size_t row, size_t col);

void lines_clear(LineBuffer *lb);

void lines_append_line(LineBuffer *lb, const char *src, size_t src_length);

void lines_insert_line(
    LineBuffer *lb, size_t i, const char *src, size_t src_length
);

void lines_insert_at(
    LineBuffer *lb, size_t row, size_t col, const char *src, size_t src_length
);

void lines_range_to_str(
    LineBuffer *lb, size_t rs, size_t cs, size_t re, size_t ce,
    char **dest, size_t *dest_length
);

void lines_delete_range(
    LineBuffer *lb, size_t rs, size_t cs, size_t re, size_t ce
);

void lines_destroy(LineBuffer *lb);

#endif // LINE_H_
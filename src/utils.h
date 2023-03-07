#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stddef.h>

size_t minul(size_t a, size_t b);

char *strdup(const char *src);

bool utils_is_word_boundary(char c);

bool utils_string_has_asterisk(char *title);

char *utils_add_asterisk_to_string(char *title);

size_t utils_find_next_line(const char *src, size_t pos, size_t src_length);

#endif // UTILS_H_

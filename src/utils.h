#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stddef.h>

size_t minul(size_t a, size_t b);

char *strdup(char *src);

bool utils_is_word_boundary(char c);

bool utils_title_has_asterisk(char *title);

void utils_add_asterisk_to_title(char **title);

void utils_remove_asterisk_from_title(char **title);

#endif // UTILS_H_

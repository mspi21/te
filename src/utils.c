#include "./utils.h"

#include <string.h>
#include <stdlib.h>

#define WORD_BOUNDARIES " \t\n\"\'()[]{}=<>!,.-_:;"

size_t minul(size_t a, size_t b) {
    return a < b ? a : b;
}

char *strdup(const char *src) {
    size_t len = strlen(src) + 1;
    char *copy = malloc(len);
    memcpy(copy, src, len);
    return copy;
}

bool utils_is_word_boundary(char c) {
    for(size_t i = 0; i < strlen(WORD_BOUNDARIES); ++i)
        if(c == WORD_BOUNDARIES[i])
            return true;
    return false;
}

static bool utils_string_has_asterisk_len(char *title, size_t len) {
    return len >= 2 && title[len - 2] == ' ' && title[len - 1] == '*';
}

bool utils_string_has_asterisk(char *title) {
    size_t len = strlen(title);
    return utils_string_has_asterisk_len(title, len);
}

char *utils_add_asterisk_to_string(char *title) {
    size_t len = strlen(title);
    title = (char *) realloc(title, len + 3);
    title[len] = ' ';
    title[len + 1] = '*';
    title[len + 2] = 0;
    return title;
}

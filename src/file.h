#ifndef FILE_H_
#define FILE_H_

#include <stdlib.h>
#include <stdbool.h>

bool file_read(const char *filepath, char **contents, size_t *length);

bool file_write(const char *filepath, char *contents, size_t length);

void file_destroy(char *contents);

#endif // FILE_H_
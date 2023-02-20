#include "./file.h"

#include <stdio.h>

size_t file_size(FILE *fp) {
    long pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return size;
}

bool file_read(
    const char *filepath,
    char **contents,
    size_t *length
) {
    FILE *fp = fopen(filepath, "r");
    if(!fp) {
        fprintf(stderr, "Error: Failed to open file %s\n", filepath);
        perror("fopen");
        return false;
    }

    *length = file_size(fp);
    *contents = (char *) malloc(*length + 1);
    if(fread(*contents, sizeof(char), *length, fp) != *length) {
        fprintf(stderr, "Error reading file %s.\n", filepath);
        goto fail;
    }
    (*contents)[*length] = 0;

    fclose(fp);
    return true;

fail:
    fclose(fp);
    return false;
}

bool file_write(
    const char *filepath,
    char *contents,
    size_t length
) {
    FILE *fp = fopen(filepath, "w");
    if(!fp) {
        fprintf(stderr, "Error: Failed to open file %s for writing\n", filepath);
        perror("fopen");
        return false;
    }

    if(fwrite(contents, sizeof(char), length, fp) != length) {
        fprintf(stderr, "Error: Could not write to file %s\n", filepath);
        goto fail;
    }

    fclose(fp);
    return true;

fail:
    fclose(fp);
    return false;
}

void file_destroy(char *contents) {
    free(contents);
}
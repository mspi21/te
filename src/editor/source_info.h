#ifndef SOURCE_INFO_H_
#define SOURCE_INFO_H_

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "../utils.h"

typedef struct {
    bool loaded_file;
    bool changed_file;
    char *filepath;
    //void (*set_title)(void *arg, const char *src);
    SDL_Window *window;
} SourceInfo;

/* SourceInfo methods */

void source_info_init(SourceInfo *si, SDL_Window *window);

bool source_info_new_file(SourceInfo *si);

void source_info_contents_changed(SourceInfo *si);

void source_info_file_loaded(SourceInfo *si, const char *filepath);

void source_info_file_saved(SourceInfo *si);

bool source_info_has_changes(SourceInfo *si);

const char *source_info_get_save_location(SourceInfo *si);

bool source_info_assure_save_location(SourceInfo *si);

bool source_info_assure_no_changes(SourceInfo *si);

void source_info_destroy(SourceInfo *si);

#endif // SOURCE_INFO_H_
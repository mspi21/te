#include "source_info.h"
#include "../dialog.h"
#include "../utils.h"

void source_info_init(SourceInfo *si, SDL_Window *window) {
    *si = (SourceInfo) {
        .loaded_file = false,
        .changed_file = false,
        .filepath = NULL,
        .window = window
    };
    SDL_SetWindowTitle(window, TITLE_DEFAULT);
}

void source_info_contents_changed(SourceInfo *si) {
    if(si->loaded_file && !si->changed_file) {
        // ... TODO set title with asterisk
    }
    si->changed_file = true;
}

bool source_info_new_file(SourceInfo *si) {
    if(si->changed_file && !dialog_confirm_unsaved_changes())
        return false; 

    free(si->filepath);
    si->filepath = NULL;
    si->changed_file = si->loaded_file = false;
    // ... TODO set title to default
    return true;
}

void source_info_file_saved(SourceInfo *si) {
    // ... TODO set title with no asterisk
    si->changed_file = false;
}

bool source_info_has_changes(SourceInfo *si) {
    return si->changed_file;
}

bool source_info_assure_save_location(SourceInfo *si) {
    if(si->loaded_file)
        return true;

    return (si->filepath = dialog_save_file());
}

const char *source_info_get_save_location(SourceInfo *si) {
    return si->filepath;
}

void source_info_file_loaded(SourceInfo *si, const char *filepath) {
    free(si->filepath);
    si->filepath = strdup(filepath);
    si->loaded_file = true;
    si->changed_file = false;
}

bool source_info_assure_no_changes(SourceInfo *si) {
    return (!si->changed_file) || dialog_confirm_unsaved_changes();
}

void source_info_destroy(SourceInfo *si) {
    free(si->filepath);
}

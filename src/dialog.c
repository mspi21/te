#include "./dialog.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_FILEPATH_LENGTH 1024

typedef enum {
    OPEN_FILE = 0,
    SAVE_FILE,
    CONFIRM_UNSAVED_CHANGES,
    COUNT_ZENITY_CMDS
} ZenityCommand;

const static char *COMMANDS[COUNT_ZENITY_CMDS] = {
    [OPEN_FILE] = "zenity --file-selection",
    [SAVE_FILE] = "zenity --file-selection --save --confirm-overwrite",
    [CONFIRM_UNSAVED_CHANGES] = "zenity --question --text \"You have unsaved changes. Do you wish to continue?\""
};

static bool zenity_read_retcode(ZenityCommand cmd) {
    FILE *fp = popen(COMMANDS[cmd], "r");
    if(!fp)
        return false;
    return pclose(fp);
}

static char *zenity_read_string(ZenityCommand cmd) {
    char buffer[MAX_FILEPATH_LENGTH];

    FILE *fp = popen(COMMANDS[cmd], "r");
    if(!fp) {
        return NULL;
    }

    fgets(buffer, sizeof(buffer), fp);
    size_t path_length = strlen(buffer);

    char *result = NULL;
    if(path_length) {
        result = (char *) malloc(path_length);
        strncpy(result, buffer, path_length - 1); // strip newline returned by command
        result[path_length - 1] = 0;
    }

    return pclose(fp) == 0 ? result : NULL;
}

bool dialog_confirm_unsaved_changes() {
    return !zenity_read_retcode(CONFIRM_UNSAVED_CHANGES);
}

char *dialog_select_file() {
    return zenity_read_string(OPEN_FILE);
}

char *dialog_select_file_default_dir(const char *dir) {
    assert(false && "TODO");
}

char *dialog_save_file() {
    return zenity_read_string(SAVE_FILE);
}

char *dialog_save_file_default_dir(const char *dir) {
    assert(false && "TODO");
}
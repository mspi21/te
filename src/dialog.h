#ifndef DIALOG_H_
#define DIALOG_H_

#include <stdbool.h>

char *dialog_select_file();

char *dialog_select_file_default_dir(const char *dir);

bool  dialog_confirm_unsaved_changes();

char *dialog_save_file();

char *dialog_save_file_default_dir(const char *dir);

#endif // DIALOG_H_
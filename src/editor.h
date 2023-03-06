#ifndef EDITOR_H_
#define EDITOR_H_

#include <stdbool.h>
#include <stddef.h>

#include <SDL2/SDL.h>

#include "editor/line.h"
#include "editor/cursor.h"
#include "editor/selection.h"
#include "editor/source_info.h"
#include "renderer.h"
#include "font.h"

typedef struct {
    SDL_Window *window;
    Renderer *renderer;
    Font *font;

    LineBuffer lines;
    Selection selection;
    SourceInfo source_info;
    Cursor cursor;
} Editor;

bool editor_init(Editor *editor, SDL_Window *window, Renderer *renderer, Font *font);

void editor_render(Editor *editor);

bool editor_load_file_from_path(Editor *editor, const char *filepath);

bool editor_load_file(Editor *editor);

bool editor_save_file(Editor *editor);

bool editor_new_file(Editor *editor);

bool editor_try_quit(Editor *editor);

void editor_destroy(Editor *editor);

void editor_insert_text_at_cursor(Editor *editor, const char *text);

void editor_delete_char_before_cursor(Editor *editor);

void editor_delete_char_after_cursor(Editor *editor);

void editor_insert_newline_at_cursor(Editor *editor);

void editor_try_copy(Editor *editor);

void editor_try_cut(Editor *editor);

void editor_handle_single_click(Editor *editor, int32_t x, int32_t y);

void editor_handle_click_release(Editor *editor);

void editor_handle_mouse_drag(Editor *editor, int x, int y);

void editor_move_cursor_right(Editor *editor);

void editor_move_cursor_left(Editor *editor);

void editor_move_cursor_up(Editor *editor);

void editor_move_cursor_down(Editor *editor);

void editor_skip_word_right(Editor *editor);

void editor_skip_word_left(Editor *editor);

void editor_swap_lines_up(Editor *editor);

void editor_swap_lines_down(Editor *editor);

void editor_grow_selection_right(Editor *editor);

void editor_grow_selection_left(Editor *editor);

void editor_grow_selection_up(Editor *editor);

void editor_grow_selection_down(Editor *editor);

void editor_scroll_x(Editor *editor, float val);

void editor_scroll_y(Editor *editor, float val);

#endif // EDITOR_H_
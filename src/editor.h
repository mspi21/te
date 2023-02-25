#ifndef EDITOR_H_
#define EDITOR_H_

#include <stdbool.h>
#include <stddef.h>

#include "./selection.h"
#include "./renderer.h"
#include "./font.h"

#include <SDL2/SDL.h>

typedef struct {
    char *buffer;
    size_t buffer_size;
    size_t buffer_capacity;
} Line;

typedef struct {
    SDL_Window *window;
    Renderer *renderer;
    Font *font;

    Line *lines;
    size_t lines_size;
    size_t lines_capacity;
    
    size_t cursor_row;
    size_t cursor_col;

    char *loaded_file;
    bool changed_file;

    bool is_selecting;
    bool has_selection;
    Selection selection;
} Editor;

bool editor_init(Editor *editor, SDL_Window *window, Renderer *renderer, Font *font);

void editor_render(Editor *editor);

bool editor_load_file_from_path(Editor *editor, char *filepath);

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

void editor_scroll_x(Editor *editor, float val);

void editor_scroll_y(Editor *editor, float val);

#endif // EDITOR_H_
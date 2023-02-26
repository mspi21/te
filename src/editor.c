#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "./file.h"
#include "./editor.h"
#include "./dialog.h"
#include "./utils.h"

#define LINE_STARTING_CAPACITY 64
#define TITLE_DEFAULT "Untitled file"

static Line line_new() {
    return (Line) {
        .buffer = (char *) malloc(LINE_STARTING_CAPACITY),
        .buffer_capacity = LINE_STARTING_CAPACITY,
        .buffer_size = 0
    };
}

static Line line_create_copy(const char *src, size_t len) {
    Line line = {
        .buffer = (char *) malloc(len),
        .buffer_capacity = len,
        .buffer_size = len
    };
    memcpy(line.buffer, src, len);
    return line;
}

static void line_grow(Line *line) {
    line->buffer_capacity = line->buffer_capacity * 2 + 64;
    line->buffer = (char *) realloc(line->buffer, line->buffer_capacity);
}

static void line_insert_text(
    Line *line, 
    size_t pos,
    const char *text,
    size_t text_length
) {
    while(line->buffer_size + text_length > line->buffer_capacity)
        line_grow(line);
    
    // Shift what was after the cursor to the right (make space for inserted text) 
    memmove(
        line->buffer + pos + text_length,
        line->buffer + pos,
        line->buffer_size - pos
    );

    // Copy text into buffer
    memcpy(
        line->buffer + pos,
        text,
        text_length
    );

    line->buffer_size += text_length;
}

static void line_delete_char(Line *line, size_t pos) {
    if(pos >= line->buffer_size)
        return;

    memmove(
        line->buffer + pos,
        line->buffer + pos + 1,
        line->buffer_size - pos
    );

    --line->buffer_size;
}

static void line_delete_range(Line *line, size_t start, size_t end) {
    if(end <= start || end > line->buffer_size)
        return;

    memmove(
        line->buffer + start,
        line->buffer + end,
        line->buffer_size - end
    );

    line->buffer_size -= end - start;
}

static void line_destroy(Line *line) {
    free(line->buffer);
    line->buffer = NULL;
}

static void editor_set_title(Editor *editor, const char *title) {
    SDL_SetWindowTitle(editor->window, title);
}

static void editor_adjust_view_to_cursor(Editor *editor) {
    float char_width = (float) editor->font->atlas.metrics['0'].advance_x;
    float line_height = (float) editor->font->atlas.height;

    float cursor_absolute_x = editor->cursor_col * char_width;
    float cursor_absolute_y = editor->cursor_row * line_height;

    int window_w, window_h;
    SDL_GetWindowSize(editor->window, &window_w, &window_h);

    if(cursor_absolute_x > window_w + editor->renderer->scroll_pos.x) {
        editor_scroll_x(editor, cursor_absolute_x - (window_w + editor->renderer->scroll_pos.x));
    }
    if(cursor_absolute_x < editor->renderer->scroll_pos.x) {
        editor_scroll_x(editor, cursor_absolute_x - editor->renderer->scroll_pos.x);
    }

    if(cursor_absolute_y > window_h + editor->renderer->scroll_pos.y) {
        editor_scroll_y(editor, cursor_absolute_y - (window_h + editor->renderer->scroll_pos.y));
    }
    if(cursor_absolute_y < editor->renderer->scroll_pos.y) {
        editor_scroll_y(editor, cursor_absolute_y - editor->renderer->scroll_pos.y);
    }
}

static void editor_contents_changed(Editor *editor) {
    if(editor->loaded_file && !editor->changed_file) {
        editor_set_title(editor, utils_add_asterisk_to_string(strdup(editor->loaded_file)));
    }
    editor->changed_file = true;
}

bool editor_init(Editor *editor, SDL_Window *window, Renderer *renderer, Font *font) {
    *editor = (Editor) {0};
    editor->window = window;
    editor->renderer = renderer;
    editor->font = font;

    editor->lines = (Line *) malloc(sizeof(Line));
    editor->lines_capacity = 1;
    editor->lines_size = 1;
    editor->lines[0] = line_new();

    editor_set_title(editor, strdup(TITLE_DEFAULT));
    return true;
}

static void editor_lines_grow(Editor *editor) {
    editor->lines_capacity = editor->lines_capacity * 2 + 32;
    editor->lines = (Line *) realloc(editor->lines, editor->lines_capacity * sizeof(Line));
}

static void editor_render_selection(Editor *editor, size_t rs, size_t cs, size_t re, size_t ce) {
    Vec4f color = vec4f(0.3f, 0.7f, 1.0f, 0.25f);
    
    int line_height = editor->font->atlas.height;
    int char_width = editor->font->atlas.metrics['0'].advance_x;

    // Selection is a single line
    if(rs == re) {
        if(cs == ce) return;
        renderer_solid_rect(
            editor->renderer,
            vec2f(cs * char_width, rs * line_height),
            vec2f((ce - cs) * char_width, (re - rs + 1) * line_height),
            color
        );
        return;
    }

    // Selection is 2+ lines
    renderer_solid_rect(
        editor->renderer,
        vec2f(cs * char_width, rs * line_height),
        vec2f((editor->lines[rs].buffer_size - cs) * char_width, line_height),
        color
    );
    for(size_t i = rs + 1; i < re; ++i)
        renderer_solid_rect(
            editor->renderer,
            vec2f(0.0f, i * line_height),
            vec2f(editor->lines[i].buffer_size * char_width, line_height),
            color
        );
    renderer_solid_rect(
        editor->renderer,
        vec2f(0.0f, re * line_height),
        vec2f(ce * char_width, line_height),
        color
    );
}

void editor_render(Editor *editor) {
    // Render selection
    if(editor->has_selection) {
        renderer_set_shader(editor->renderer, SHADER_SOLID);
        {
            size_t srow_start, scol_start, srow_end, scol_end;
            selection_get_ordered_range(
                &editor->selection,
                &srow_start, &scol_start, &srow_end, &scol_end
            );
            editor_render_selection(editor, srow_start, scol_start, srow_end, scol_end);
        }
        renderer_flush(editor->renderer);
    }

    // Render text
    int line_height = editor->font->atlas.height;
    for(size_t i = 0; i < editor->lines_size; ++i) {
        font_render_line(
            editor->font,
            editor->renderer,
            editor->lines[i].buffer,
            editor->lines[i].buffer_size,
            vec2f(0.0f, (float)((i + 1) * line_height)),
            vec4f(0.0f, 0.0f, 0.0f, 1.0f)
        );
        
        // Render cursor
        if(editor->cursor_row == i) {
            float x_pos = font_calculate_width(
                editor->font,
                editor->lines[i].buffer,
                editor->cursor_col
            );
            float y_pos = (float) (i * line_height);
            renderer_set_shader(editor->renderer, SHADER_SOLID);
            renderer_solid_rect(editor->renderer,
                vec2f(x_pos, y_pos),
                vec2f(2.0f, line_height),
                vec4f(0.0f, 0.0f, 0.0f, 1.0f)
            );
            renderer_flush(editor->renderer);
        }
    }
}

static void editor_add_line(Editor *editor, const char *src, size_t len) {
    if(editor->lines_size == editor->lines_capacity)
        editor_lines_grow(editor);
    editor->lines[editor->lines_size++] = line_create_copy(src, len);
}

static void editor_clear_lines(Editor *editor) {
    for(size_t i = 0; i < editor->lines_size; ++i) {
        line_destroy(&editor->lines[i]);
    }
    editor->lines = NULL;
    editor->lines_size = 0;
    editor->lines_capacity = 0;
}

static void editor_clamp_cursor(Editor *editor) {
    if(editor->cursor_col > editor->lines[editor->cursor_row].buffer_size) {
        editor->cursor_col = editor->lines[editor->cursor_row].buffer_size;
    }
}

bool editor_load_file_from_path(Editor *editor, char *filepath) {
    char *buffer;
    size_t length;
    
    if(!file_read(filepath, &buffer, &length)) {
        return false;
    }

    editor_clear_lines(editor);
    size_t line_start = 0;
    size_t line_length = 0;
    for(size_t i = 0; i < length; ++i) {
        if(buffer[i] == '\n') {
            editor_add_line(editor, buffer + line_start, line_length);
            line_start = i + 1;
            line_length = 0;
        }
        else {
            ++line_length;
        }
    }
    editor_add_line(editor, buffer + line_start, line_length);
    
    free(editor->loaded_file);
    editor->loaded_file = filepath;
    editor->changed_file = false;
    editor_set_title(editor, filepath);

    editor->renderer->scroll_pos = vec2f(0.0f, 0.0f);
    editor->cursor_row = 0;
    editor->cursor_col = 0;
    return true;
}

bool editor_load_file(Editor *editor) {
    if(editor->changed_file && !dialog_confirm_unsaved_changes())
        return false;

    char *selected_file = dialog_select_file();
    if(!selected_file)
        return false;

    return editor_load_file_from_path(editor, selected_file);
}

static void editor_get_range_as_str(
    Editor *editor,
    size_t rs, size_t cs,
    size_t re, size_t ce,
    char **buffer_ptr,
    size_t *len_ptr
) {
    // Range is on a single line
    if(rs == re) {
        *len_ptr = ce - cs;
        *buffer_ptr = (char *) malloc(*len_ptr + 1);
        memcpy(*buffer_ptr, editor->lines[rs].buffer + cs, ce - cs);
        (*buffer_ptr)[*len_ptr] = 0;
        return;
    }

    // Range covers 2+ lines
    *len_ptr = 0;
    *len_ptr += editor->lines[rs].buffer_size - cs + 1; // newline
    for(size_t i = rs + 1; i < re; ++i)
        *len_ptr += editor->lines[i].buffer_size + 1; // newline
    *len_ptr += ce;

    *buffer_ptr = (char *) malloc(*len_ptr + 1); // null terminator
    size_t buffer_pos = 0;

    memcpy(*buffer_ptr + buffer_pos, editor->lines[rs].buffer + cs, editor->lines[rs].buffer_size - cs);
    buffer_pos += editor->lines[rs].buffer_size - cs;
    (*buffer_ptr)[buffer_pos++] = '\n';

    for(size_t i = rs + 1; i < re; ++i) {
        memcpy(*buffer_ptr + buffer_pos, editor->lines[i].buffer, editor->lines[i].buffer_size);
        buffer_pos += editor->lines[i].buffer_size;
        (*buffer_ptr)[buffer_pos++] = '\n';
    }

    memcpy(*buffer_ptr + buffer_pos, editor->lines[re].buffer, ce);
    buffer_pos += ce;
    buffer_ptr[buffer_pos] = 0;
    assert(buffer_pos == *len_ptr);
}

bool editor_save_file(Editor *editor) {
    if(!editor->changed_file)
        return true;

    char *dest = editor->loaded_file;
    if(!dest) {
        dest = dialog_save_file();
        if(!dest)
            return false;
    }

    char *buffer;
    size_t buffer_length;
    editor_get_range_as_str(
        editor,
        0,
        0,
        editor->lines_size - 1,
        editor->lines[editor->lines_size - 1].buffer_size,
        &buffer,
        &buffer_length
    );

    if(!file_write(dest, buffer, buffer_length))
        goto fail;

    editor->loaded_file = dest;
    editor->changed_file = false;
    editor_set_title(editor, editor->loaded_file);

    free(buffer);
    return true;
fail:
    free(buffer);
    return false;
}

bool editor_new_file(Editor *editor) {
    if(editor->loaded_file
    && editor->changed_file
    && !dialog_confirm_unsaved_changes())
        return false;
    
    free(editor->loaded_file);
    editor->loaded_file = NULL;
    editor->changed_file = false;

    editor_clear_lines(editor);
    editor_add_line(editor, "", 0);

    editor_set_title(editor, TITLE_DEFAULT);

    editor->renderer->scroll_pos = vec2f(0.0f, 0.0f);
    editor->cursor_row = 0;
    editor->cursor_col = 0;
    return true;
}

static void editor_remove_selection(Editor *editor) {
    size_t rs, cs, re, ce;
    selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
    if(!rs && !cs && !re && !ce)
        return;

    // Selection is a single line
    if(rs == re) {
        line_delete_range(&editor->lines[rs], cs, ce);
        goto epilog;
    }

    // Selection is 2+ lines
    line_delete_range(&editor->lines[rs], cs, editor->lines[rs].buffer_size);
    line_insert_text(
        &editor->lines[rs],
        editor->lines[rs].buffer_size,
        editor->lines[re].buffer + ce,
        editor->lines[re].buffer_size - ce
    );
    for(size_t i = rs + 1; i <= re; ++i)
        line_destroy(&editor->lines[i]);
    if(re != editor->lines_size - 1)
        memmove(
            editor->lines + rs + 1,
            editor->lines + re + 1,
            (editor->lines_size - (re + 1)) * sizeof(*editor->lines)
        );
    editor->lines_size -= re - rs;

epilog:
    selection_reset(&editor->selection);
    editor->cursor_col = cs;
    return;
}

void editor_insert_text_at_cursor(Editor *editor, const char *text) {
    editor_remove_selection(editor);
    editor->has_selection = false;

    size_t text_length = strlen(text);

    Line *line = &editor->lines[editor->cursor_row];
    line_insert_text(line, editor->cursor_col, text, text_length);
    editor_contents_changed(editor);

    editor->cursor_col += text_length;
    editor_adjust_view_to_cursor(editor);
}

void editor_try_copy(Editor *editor) {
    if(!editor->has_selection)
        return;
    size_t rs, cs, re, ce;
    selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
    
    char *text;
    size_t text_length;
    editor_get_range_as_str(editor, rs, cs, re, ce, &text, &text_length);
    
    SDL_SetClipboardText(text); // TODO error checking
    free(text);
}

void editor_try_cut(Editor *editor) {
    if(!editor->has_selection)
        return;
    editor_try_copy(editor);
    editor_remove_selection(editor);
}

void editor_delete_char_before_cursor(Editor *editor) {
    if(editor->has_selection) {
        editor_remove_selection(editor);
        editor->has_selection = false;
        return;
    }

    if(editor->cursor_col) {
        Line *line = &editor->lines[editor->cursor_row];
        line_delete_char(line, --editor->cursor_col);
        editor_contents_changed(editor);

        editor_adjust_view_to_cursor(editor);
        return;
    }

    if(!editor->cursor_row)
        return;

    Line *this_line = &editor->lines[editor->cursor_row];
    Line *prev_line = &editor->lines[editor->cursor_row - 1];
    size_t prev_line_end = prev_line->buffer_size;

    line_insert_text(prev_line, prev_line->buffer_size, this_line->buffer, this_line->buffer_size);
    line_destroy(this_line);
    memmove(
        editor->lines + editor->cursor_row,
        editor->lines + editor->cursor_row + 1,
        (editor->lines_size - editor->cursor_row - 1) * sizeof(*editor->lines)
    );
    --editor->lines_size;
    editor_contents_changed(editor);
    
    --editor->cursor_row;
    editor->cursor_col = prev_line_end;
    editor_adjust_view_to_cursor(editor);
}

void editor_delete_char_after_cursor(Editor *editor) {
    if(editor->has_selection) {
        editor_remove_selection(editor);
        editor->has_selection = false;
        return;
    }

    if(editor->cursor_col < editor->lines[editor->cursor_row].buffer_size) {
        Line *line = &editor->lines[editor->cursor_row];
        line_delete_char(line, editor->cursor_col);
        editor_contents_changed(editor);
    
        editor_adjust_view_to_cursor(editor);
        return;
    }

    if(editor->cursor_row == editor->lines_size - 1)
        return;

    Line *this_line = &editor->lines[editor->cursor_row];
    Line *next_line = &editor->lines[editor->cursor_row + 1];
    line_insert_text(this_line, this_line->buffer_size, next_line->buffer, next_line->buffer_size);
    line_destroy(next_line);
    memmove(
        editor->lines + editor->cursor_row + 1,
        editor->lines + editor->cursor_row + 2,
        (editor->lines_size - editor->cursor_row + 2) * sizeof(*editor->lines)
    );
    --editor->lines_size;
    editor_contents_changed(editor);
    
    editor_adjust_view_to_cursor(editor);
}

void editor_insert_newline_at_cursor(Editor *editor) {
    editor_remove_selection(editor);
    if(editor->lines_size == editor->lines_capacity)
        editor_lines_grow(editor);
    
    Line *this_line = &editor->lines[editor->cursor_row];
    
    Line new_line = line_create_copy(
        this_line->buffer + editor->cursor_col,
        this_line->buffer_size - editor->cursor_col
    );
    this_line->buffer_size = editor->cursor_col;

    memmove(
        editor->lines + editor->cursor_row + 2,
        editor->lines + editor->cursor_row + 1,
        (editor->lines_size - editor->cursor_row - 1) * sizeof(*editor->lines)
    );

    ++editor->lines_size;
    editor->lines[++editor->cursor_row] = new_line;
    editor->cursor_col = 0;
    editor_contents_changed(editor);

    editor_adjust_view_to_cursor(editor);
}

static void editor_get_cursor_pos_from_coords(Editor *editor, int32_t x, int32_t y, size_t *row, size_t *col) {
    float line_height = (float) editor->font->atlas.height;
    // since we're using a monospace font, all characters should have the same advance_x
    float char_width = (float) editor->font->atlas.metrics['0'].advance_x;
    Vec2f scroll_pos = editor->renderer->scroll_pos;

    *row = minul(
        (scroll_pos.y + y) / line_height,
        editor->lines_size - 1
    );
    *col = minul(
        (scroll_pos.x + x + (char_width / 2)) / char_width,
        editor->lines[*row].buffer_size
    );
}

void editor_handle_single_click(Editor *editor, int32_t x, int32_t y) {
    editor_get_cursor_pos_from_coords(editor, x, y, &editor->cursor_row, &editor->cursor_col);
    editor->has_selection = false;
    editor->is_selecting = true;
    selection_start_selecting(&editor->selection, editor->cursor_row, editor->cursor_col);
    editor_adjust_view_to_cursor(editor);
}

void editor_handle_click_release(Editor *editor) {
    editor->is_selecting = false;
}

void editor_handle_mouse_drag(Editor *editor, int32_t x, int32_t y) {
    if(!editor->is_selecting)
        return;

    size_t row, col;
    editor_get_cursor_pos_from_coords(editor, x, y, &row, &col);
    if(row != editor->cursor_row || col != editor->cursor_col) {
        editor->has_selection = true;
        editor->cursor_row = row;
        editor->cursor_col = col;
        selection_update_selection(&editor->selection, row, col);
    }
}

void editor_move_cursor_right(Editor *editor) {
    if(editor->has_selection) {
        size_t rs, cs, re, ce;
        selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
        editor->cursor_row = re;
        editor->cursor_col = ce;
        selection_reset(&editor->selection);
        editor->has_selection = false;
        return;
    }

    if(editor->cursor_col < editor->lines[editor->cursor_row].buffer_size) {
        ++editor->cursor_col;
    }

    else if(editor->cursor_row < editor->lines_size - 1) {
        ++editor->cursor_row;
        editor->cursor_col = 0;
    }

    editor_adjust_view_to_cursor(editor);
}

void editor_move_cursor_left(Editor *editor) {
    if(editor->has_selection) {
        size_t rs, cs, re, ce;
        selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
        editor->cursor_row = rs;
        editor->cursor_col = cs;
        selection_reset(&editor->selection);
        editor->has_selection = false;
        return;
    }

    if(editor->cursor_col) --editor->cursor_col;

    else if(editor->cursor_row) {
        --editor->cursor_row;
        editor->cursor_col = editor->lines[editor->cursor_row].buffer_size;
    }

    editor_adjust_view_to_cursor(editor);
}

void editor_move_cursor_up(Editor *editor) {
    if(editor->has_selection) {
        selection_reset(&editor->selection);
        editor->has_selection = false;
    }

    if(editor->cursor_row) {
        --editor->cursor_row;
        editor_clamp_cursor(editor);
    }
    else editor->cursor_col = 0;

    editor_adjust_view_to_cursor(editor);
}

void editor_move_cursor_down(Editor *editor) {
    if(editor->has_selection) {
        selection_reset(&editor->selection);
        editor->has_selection = false;
    }

    if(editor->cursor_row < editor->lines_size - 1) {
        ++editor->cursor_row;
        editor_clamp_cursor(editor);
    }
    else editor->cursor_col = editor->lines[editor->cursor_row].buffer_size;

    editor_adjust_view_to_cursor(editor);
}

void editor_skip_word_right(Editor *editor) {
    if(editor->has_selection) {
        selection_reset(&editor->selection);
        editor->has_selection = false;
    }

    if(editor->cursor_col == editor->lines[editor->cursor_row].buffer_size) {
        editor_move_cursor_right(editor);
        return;
    }

    while(
        editor->cursor_col < editor->lines[editor->cursor_row].buffer_size &&
        utils_is_word_boundary(editor->lines[editor->cursor_row].buffer[editor->cursor_col])
    )
        ++editor->cursor_col;

    while(
        editor->cursor_col < editor->lines[editor->cursor_row].buffer_size &&
        !utils_is_word_boundary(editor->lines[editor->cursor_row].buffer[editor->cursor_col])
    )
        ++editor->cursor_col;

    editor_adjust_view_to_cursor(editor);
}

void editor_skip_word_left(Editor *editor) {
    if(editor->has_selection) {
        selection_reset(&editor->selection);
        editor->has_selection = false;
    }

    if(!editor->cursor_col) {
        editor_move_cursor_left(editor);
        return;
    }

    while(
        editor->cursor_col > 0 &&
        utils_is_word_boundary(editor->lines[editor->cursor_row].buffer[editor->cursor_col - 1])
    )
        --editor->cursor_col;

    while(
        editor->cursor_col > 0 &&
        !utils_is_word_boundary(editor->lines[editor->cursor_row].buffer[editor->cursor_col - 1])
    )
        --editor->cursor_col;

    editor_adjust_view_to_cursor(editor);
}

void editor_swap_lines_up(Editor *editor) {
    if(!editor->cursor_row)
        return;

    Line tmp = editor->lines[editor->cursor_row];
    editor->lines[editor->cursor_row] = editor->lines[editor->cursor_row - 1];
    editor->lines[editor->cursor_row - 1] = tmp;
    editor_contents_changed(editor);

    --editor->cursor_row;    
    editor_adjust_view_to_cursor(editor);
}

void editor_swap_lines_down(Editor *editor) {
    if(editor->cursor_row == editor->lines_size - 1)
        return;

    Line tmp = editor->lines[editor->cursor_row];
    editor->lines[editor->cursor_row] = editor->lines[editor->cursor_row + 1];
    editor->lines[editor->cursor_row + 1] = tmp;
    editor_contents_changed(editor);

    ++editor->cursor_row;
    editor_adjust_view_to_cursor(editor);
}

void editor_grow_selection_right(Editor *editor) {

}

void editor_grow_selection_left(Editor *editor) {

}

void editor_grow_selection_up(Editor *editor) {

}

void editor_grow_selection_down(Editor *editor) {

}

void editor_scroll_x(Editor *editor, float val) {
    editor->renderer->scroll_pos.x += /*SCROLL_SPEED * */val;
    if(editor->renderer->scroll_pos.x < 0.0f)
        editor->renderer->scroll_pos.x = 0.0f;
}

void editor_scroll_y(Editor *editor, float val) {
    editor->renderer->scroll_pos.y += /*SCROLL_INVERTED * SCROLL_SPEED * */val;
    if(editor->renderer->scroll_pos.y < 0.0f)
        editor->renderer->scroll_pos.y = 0.0f;
}

bool editor_try_quit(Editor *editor) {
    if(!editor->changed_file)
        return true;
    return dialog_confirm_unsaved_changes();
}

void editor_destroy(Editor *editor) {
    for(size_t i = 0; i < editor->lines_size; ++i) {
        line_destroy(&editor->lines[i]);
    }
    free(editor->lines);
    editor->lines = NULL;

    free(editor->loaded_file);
}

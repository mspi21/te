#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "file.h"
#include "editor.h"
#include "dialog.h"
#include "utils.h"

static void editor_adjust_view_to_cursor(Editor *editor) {
    float char_width = (float) editor->font->atlas.metrics['0'].advance_x;
    float line_height = (float) editor->font->atlas.height;

    float cursor_absolute_x = editor->cursor.col * char_width;
    float cursor_absolute_y = editor->cursor.row * line_height;

    int window_w, window_h;
    SDL_GetWindowSize(editor->window, &window_w, &window_h);

    if(cursor_absolute_x > window_w + editor->renderer->scroll_pos.x) {
        editor_scroll_x(editor, cursor_absolute_x - (window_w + editor->renderer->scroll_pos.x));
    }
    else if(cursor_absolute_x < editor->renderer->scroll_pos.x) {
        editor_scroll_x(editor, cursor_absolute_x - editor->renderer->scroll_pos.x);
    }

    if(cursor_absolute_y > window_h + editor->renderer->scroll_pos.y) {
        editor_scroll_y(editor, cursor_absolute_y - (window_h + editor->renderer->scroll_pos.y));
    }
    else if(cursor_absolute_y < editor->renderer->scroll_pos.y) {
        editor_scroll_y(editor, cursor_absolute_y - editor->renderer->scroll_pos.y);
    }
}

bool editor_init(Editor *editor, SDL_Window *window, Renderer *renderer, Font *font) {
    *editor = (Editor) {0};
    editor->window = window;
    editor->renderer = renderer;
    editor->font = font;

    lines_create(&editor->lines);
    lines_append_line(&editor->lines, "", 0);
    cursor_init(&editor->cursor);
    source_info_init(&editor->source_info, editor->window);

    return true;
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
        vec2f((editor->lines.lines[rs].buffer_size - cs) * char_width, line_height),
        color
    );
    for(size_t i = rs + 1; i < re; ++i)
        renderer_solid_rect(
            editor->renderer,
            vec2f(0.0f, i * line_height),
            vec2f(editor->lines.lines[i].buffer_size * char_width, line_height),
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
    if(selection_is_nonempty(&editor->selection)) {
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
    for(size_t i = 0; i < editor->lines.lines_size; ++i) {
        font_render_line(
            editor->font,
            editor->renderer,
            editor->lines.lines[i].buffer,
            editor->lines.lines[i].buffer_size,
            vec2f(0.0f, (float)((i + 1) * line_height)),
            vec4f(0.0f, 0.0f, 0.0f, 1.0f)
        );
        
        // Render cursor
        if(editor->cursor.row == i) {
            float x_pos = font_calculate_width(
                editor->font,
                editor->lines.lines[i].buffer,
                editor->cursor.col
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

// TODO finish refactor
bool editor_load_file_from_path(Editor *editor, const char *filepath) {
    char *buffer;
    size_t length;
    
    if(!file_read(filepath, &buffer, &length)) {
        return false;
    }

    lines_clear(&editor->lines);
    size_t line_start = 0;
    size_t line_length = 0;
    for(size_t i = 0; i < length; ++i) {
        if(buffer[i] == '\n') {
            lines_append_line(&editor->lines, buffer + line_start, line_length);
            line_start = i + 1;
            line_length = 0;
        }
        else {
            ++line_length;
        }
    }
    lines_append_line(&editor->lines, buffer + line_start, line_length);
    
    source_info_file_loaded(&editor->source_info, filepath);

    editor->renderer->scroll_pos = vec2f(0.0f, 0.0f);
    cursor_set(&editor->cursor, &editor->lines, 0, 0);
    return true;
}

// TODO finish refactor
bool editor_load_file(Editor *editor) {
    if(editor->source_info.changed_file && !dialog_confirm_unsaved_changes())
        return false;

    char *selected_file = dialog_select_file();
    if(!selected_file)
        return false;

    bool ret = editor_load_file_from_path(editor, selected_file);
    free(selected_file);
    return ret;
}

bool editor_save_file(Editor *editor) {
    if(!source_info_has_changes(&editor->source_info))
        return true;

    if(!source_info_assure_save_location(&editor->source_info))
        return false;

    char *buffer;
    size_t buffer_length;

    lines_range_to_str(
        &editor->lines,
        0, 0,
        editor->lines.lines_size - 1,
        editor->lines.lines[editor->lines.lines_size - 1].buffer_size,
        &buffer, &buffer_length
    );

    if(!file_write(
        source_info_get_save_location(&editor->source_info),
        buffer,
        buffer_length
    ))
        goto fail;

    source_info_file_saved(&editor->source_info);

    free(buffer);
    return true;
fail:
    free(buffer);
    return false;
}

bool editor_new_file(Editor *editor) {
    if(!source_info_new_file(&editor->source_info))
        return false;
    
    lines_clear(&editor->lines);
    lines_append_line(&editor->lines, "", 0);

    editor->renderer->scroll_pos = vec2f(0.0f, 0.0f);
    cursor_set(&editor->cursor, &editor->lines, 0, 0);
    return true;
}

static void editor_remove_selection(Editor *editor) {
    size_t rs, cs, re, ce;
    selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
    if(!selection_is_nonempty(&editor->selection))
        return;

    lines_delete_range(&editor->lines, rs, cs, re, ce);
    selection_reset(&editor->selection);
    // TODO row = rs?
    editor->cursor.col = cs;
    return;
}

void editor_insert_text_at_cursor(Editor *editor, const char *text) {
    editor_remove_selection(editor);

    size_t text_length = strlen(text);
    lines_insert_at(&editor->lines, editor->cursor.row, editor->cursor.col, text, text_length);
    source_info_contents_changed(&editor->source_info);

    cursor_advance(&editor->cursor, &editor->lines, text_length);
    editor_adjust_view_to_cursor(editor);
}

void editor_try_copy(Editor *editor) {
    if(!selection_is_nonempty(&editor->selection))
        return;
    size_t rs, cs, re, ce;
    selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
    
    char *text;
    size_t text_length;
    lines_range_to_str(&editor->lines, rs, cs, re, ce, &text, &text_length);
    
    SDL_SetClipboardText(text); // TODO error checking
    free(text);
}

void editor_try_cut(Editor *editor) {
    if(!selection_is_nonempty(&editor->selection))
        return;
    editor_try_copy(editor);
    editor_remove_selection(editor);
}

void editor_delete_char_before_cursor(Editor *editor) {
    if(selection_is_nonempty(&editor->selection)) {
        editor_remove_selection(editor);
        return;
    }

    if(editor->cursor.col) {
        lines_delete_range(
            &editor->lines,
            editor->cursor.row, editor->cursor.col - 1,
            editor->cursor.row, editor->cursor.col
        );
        --editor->cursor.col;
        goto epilog;
    }

    if(!editor->cursor.row)
        return;

    Line *this_line = &editor->lines.lines[editor->cursor.row];
    Line *prev_line = &editor->lines.lines[editor->cursor.row - 1];
    size_t prev_line_end = prev_line->buffer_size;

    line_insert_text(prev_line, prev_line->buffer_size, this_line->buffer, this_line->buffer_size);
    line_destroy(this_line);
    lines_move_raw(
        &editor->lines,
        editor->cursor.row + 1,
        editor->cursor.row,
        editor->lines.lines_size - (editor->cursor.row + 1)
    );
    --editor->lines.lines_size;
    
    --editor->cursor.row;
    editor->cursor.col = prev_line_end;

epilog:
    source_info_contents_changed(&editor->source_info);
    editor_adjust_view_to_cursor(editor);
}

void editor_delete_char_after_cursor(Editor *editor) {
    if(selection_is_nonempty(&editor->selection)) {
        editor_remove_selection(editor);
        return;
    }

    if(editor->cursor.col < editor->lines.lines[editor->cursor.row].buffer_size) {
        lines_delete_range(
            &editor->lines,
            editor->cursor.row, editor->cursor.col,
            editor->cursor.row, editor->cursor.col + 1
        );
        goto epilog;
    }

    if(editor->cursor.row == editor->lines.lines_size - 1)
        return;

    Line *this_line = &editor->lines.lines[editor->cursor.row];
    Line *next_line = &editor->lines.lines[editor->cursor.row + 1];
    line_insert_text(this_line, this_line->buffer_size, next_line->buffer, next_line->buffer_size);
    line_destroy(next_line);
    lines_move_raw(
        &editor->lines,
        editor->cursor.row + 2,
        editor->cursor.row + 1,
        editor->lines.lines_size - (editor->cursor.row + 2)
    );
    --editor->lines.lines_size;
    
epilog:
    source_info_contents_changed(&editor->source_info);
    editor_adjust_view_to_cursor(editor);
}

void editor_insert_newline_at_cursor(Editor *editor) {
    editor_remove_selection(editor);
    lines_split(&editor->lines, editor->cursor.row, editor->cursor.col);

    ++editor->cursor.row;
    editor->cursor.col = 0;
    source_info_contents_changed(&editor->source_info);
    editor_adjust_view_to_cursor(editor);
}

static void editor_get_cursor_pos_from_coords(Editor *editor, int32_t x, int32_t y, size_t *row, size_t *col) {
    float line_height = (float) editor->font->atlas.height;
    // since we're using a monospace font, all characters should have the same advance_x
    float char_width = (float) editor->font->atlas.metrics['0'].advance_x;
    Vec2f scroll_pos = editor->renderer->scroll_pos;

    *row = minul(
        (scroll_pos.y + y) / line_height,
        editor->lines.lines_size - 1
    );
    *col = minul(
        (scroll_pos.x + x + (char_width / 2)) / char_width,
        editor->lines.lines[*row].buffer_size
    );
}

void editor_handle_single_click(Editor *editor, int32_t x, int32_t y) {
    editor_get_cursor_pos_from_coords(editor, x, y, &editor->cursor.row, &editor->cursor.col);
    selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
    editor_adjust_view_to_cursor(editor);
}

void editor_handle_click_release(Editor *editor) {
    selection_stop_selecting(&editor->selection);
}

void editor_handle_mouse_drag(Editor *editor, int32_t x, int32_t y) {
    if(!selection_is_selecting(&editor->selection))
        return;

    size_t row, col;
    editor_get_cursor_pos_from_coords(editor, x, y, &row, &col);
    if(row != editor->cursor.row || col != editor->cursor.col) {
        cursor_set(&editor->cursor, &editor->lines, row, col);
        selection_update_selection(&editor->selection, row, col);
        editor_adjust_view_to_cursor(editor);
    }
}

void editor_move_cursor_right(Editor *editor) {
    if(selection_is_nonempty(&editor->selection)) {
        size_t rs, cs, re, ce;
        selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
        cursor_set(&editor->cursor, &editor->lines, re, ce);
        selection_reset(&editor->selection);
        return;
    }

    if(cursor_move_right(&editor->cursor, &editor->lines))
        editor_adjust_view_to_cursor(editor);
}

void editor_move_cursor_left(Editor *editor) {
    if(selection_is_nonempty(&editor->selection)) {
        size_t rs, cs, re, ce;
        selection_get_ordered_range(&editor->selection, &rs, &cs, &re, &ce);
        cursor_set(&editor->cursor, &editor->lines, rs, cs);
        selection_reset(&editor->selection);
        return;
    }

    if(cursor_move_left(&editor->cursor, &editor->lines))
        editor_adjust_view_to_cursor(editor);
}

void editor_move_cursor_up(Editor *editor) {
    selection_reset(&editor->selection);
    if(cursor_move_up(&editor->cursor, &editor->lines))
        editor_adjust_view_to_cursor(editor);
}

void editor_move_cursor_down(Editor *editor) {
    selection_reset(&editor->selection);
    if(cursor_move_down(&editor->cursor, &editor->lines))
        editor_adjust_view_to_cursor(editor);
}

void editor_skip_word_right(Editor *editor) {
    selection_reset(&editor->selection);
    if(cursor_skip_word_right(&editor->cursor, &editor->lines))
        editor_adjust_view_to_cursor(editor);
}

void editor_skip_word_left(Editor *editor) {
    selection_reset(&editor->selection);
    if(cursor_skip_word_left(&editor->cursor, &editor->lines))
        editor_adjust_view_to_cursor(editor);
}

void editor_swap_lines_up(Editor *editor) {
    if(!editor->cursor.row)
        return;

    lines_swap(&editor->lines, editor->cursor.row - 1, editor->cursor.row);

    --editor->cursor.row;
    source_info_contents_changed(&editor->source_info);
    editor_adjust_view_to_cursor(editor);
}

void editor_swap_lines_down(Editor *editor) {
    if(editor->cursor.row == editor->lines.lines_size - 1)
        return;

    lines_swap(&editor->lines, editor->cursor.row, editor->cursor.row + 1);
    
    ++editor->cursor.row;
    source_info_contents_changed(&editor->source_info);
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
    return source_info_assure_no_changes(&editor->source_info);
}

void editor_destroy(Editor *editor) {
    lines_destroy(&editor->lines);
    cursor_destroy(&editor->cursor);
    source_info_destroy(&editor->source_info);
}

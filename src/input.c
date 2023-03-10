#include "./input.h"
#include "./editor.h"

#include <stdio.h>

#define SCROLL_SPEED 60.0f
#define SCROLL_INVERTED -1

#define TEXT_TAB "    "

// TODO make an input structure
static unsigned long is_shift_down = 0;

static void handle_textinput(SDL_TextInputEvent *text, Editor *editor) {
    editor_insert_text_at_cursor(editor, text->text);
}

static void handle_mouse_button_press(SDL_MouseButtonEvent *button, Editor *editor) {
    if(button->button == SDL_BUTTON_LEFT && button->clicks == 1) {
        if(is_shift_down)
            editor_handle_shift_click(editor, button->x, button->y);
        else
            editor_handle_single_click(editor, button->x, button->y);
    }
}

static void handle_mouse_button_release(SDL_MouseButtonEvent *button, Editor *editor) {
    if(button->button == SDL_BUTTON_LEFT) {
        editor_handle_click_release(editor);
    }
}

static void handle_mouse_drag(SDL_MouseMotionEvent *motion, Editor *editor) {
    if(motion->state == SDL_BUTTON_LMASK) {
        editor_handle_mouse_drag(editor, motion->x, motion->y);
    }
}

static void handle_mouse_wheel(SDL_MouseWheelEvent *wheel, Editor *editor) {
    if(wheel->x)
        editor_scroll_x(
            editor,
            SCROLL_SPEED * wheel->preciseX
        );
    if(wheel->y)
        editor_scroll_y(
            editor,
            SCROLL_INVERTED * SCROLL_SPEED * wheel->preciseY
        );
}

static void handle_ctrl_shift_and_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        // TODO obviously...
        case SDLK_RIGHT: {
            if(!selection_is_nonempty(&editor->selection))
                selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
            editor_skip_word_right(editor);
            selection_update_selection(&editor->selection, editor->cursor.row, editor->cursor.col);
            selection_stop_selecting(&editor->selection);
        } break;
        case SDLK_LEFT: {
            if(!selection_is_nonempty(&editor->selection))
                selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
            editor_skip_word_left(editor);
            selection_update_selection(&editor->selection, editor->cursor.row, editor->cursor.col);
            selection_stop_selecting(&editor->selection);
        } break;
        case SDLK_UP:   { } break;
        case SDLK_DOWN: { } break;
        default: return;
    }
}

static void handle_ctrl_and_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        case SDLK_RIGHT: {
            selection_reset(&editor->selection);
            editor_skip_word_right(editor);
        } break;
        case SDLK_LEFT:  {
            selection_reset(&editor->selection);
            editor_skip_word_left(editor);
        } break;
        case SDLK_UP:    {
            selection_reset(&editor->selection);
            editor_swap_lines_up(editor);
        } break;
        case SDLK_DOWN:  {
            selection_reset(&editor->selection);
            editor_swap_lines_down(editor);
        } break;

        case SDLK_a: { editor_select_all(editor); } break;
        case SDLK_o: { editor_load_file(editor); } break;
        case SDLK_s: { editor_save_file(editor); } break;
        case SDLK_n: { editor_new_file(editor); } break;
        // Quickfix, will probably remove later
        case SDLK_e: {
            editor_handle_single_click(editor, 0, 1 << 31);
        } break;
        case SDLK_c: { editor_try_copy(editor); } break;
        case SDLK_x: { editor_try_cut(editor); } break;
        case SDLK_v: {
            if(SDL_HasClipboardText() == SDL_TRUE) {
                char *clipboard = SDL_GetClipboardText();
                //printf("Debug: Clipboard text:\n===\n%s\n===\n", clipboard);
                editor_insert_text_at_cursor(editor, clipboard);
                SDL_free(clipboard);
            }
        } break;
        default: return;
    }
}

static void handle_shift_and_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        // TODO obviously...
        case SDLK_RIGHT: {
            if(!selection_is_nonempty(&editor->selection))
                selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
            editor_move_cursor_right(editor);
            selection_update_selection(&editor->selection, editor->cursor.row, editor->cursor.col);
            selection_stop_selecting(&editor->selection);
        } break;
        case SDLK_LEFT:  {
            if(!selection_is_nonempty(&editor->selection))
                selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
            editor_move_cursor_left(editor);
            selection_update_selection(&editor->selection, editor->cursor.row, editor->cursor.col);
            selection_stop_selecting(&editor->selection);
        } break;
        case SDLK_UP:    {
            if(!selection_is_nonempty(&editor->selection))
                selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
            editor_move_cursor_up(editor);
            selection_update_selection(&editor->selection, editor->cursor.row, editor->cursor.col);
            selection_stop_selecting(&editor->selection);
        } break;
        case SDLK_DOWN:  {
            if(!selection_is_nonempty(&editor->selection))
                selection_start_selecting(&editor->selection, editor->cursor.row, editor->cursor.col);
            editor_move_cursor_down(editor);
            selection_update_selection(&editor->selection, editor->cursor.row, editor->cursor.col);
            selection_stop_selecting(&editor->selection);
        } break;
        default: return;
    }
}

static void handle_key_down_no_mod(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        case SDLK_RIGHT: {
            editor_deselect_and_set_cursor_after(editor);
            editor_move_cursor_right(editor);
        } break;
        case SDLK_LEFT: {
            editor_deselect_and_set_cursor_before(editor);
            editor_move_cursor_left(editor);
        } break;
        case SDLK_UP: {
            selection_reset(&editor->selection);
            editor_move_cursor_up(editor);
        } break;
        case SDLK_DOWN: {
            selection_reset(&editor->selection);
            editor_move_cursor_down(editor);
        } break;

        case SDLK_BACKSPACE: { editor_delete_char_before_cursor(editor); } break;
        case SDLK_DELETE: { editor_delete_char_after_cursor(editor); } break;
        case SDLK_RETURN: { editor_insert_newline_at_cursor(editor); } break;
        case SDLK_TAB: { editor_insert_text_at_cursor(editor, TEXT_TAB); } break;
        default: return;
    }
}

static void handle_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    if(key->keysym.sym == SDLK_LSHIFT || key->keysym.sym == SDLK_RSHIFT) {
        is_shift_down = 1;
        return;
    }

    if((key->keysym.mod & KMOD_CTRL) && (key->keysym.mod & KMOD_SHIFT)) {
        handle_ctrl_shift_and_key_down(key, editor);
        return;
    }

    if(key->keysym.mod & KMOD_CTRL) {
        handle_ctrl_and_key_down(key, editor);
        return;
    }

    if(key->keysym.mod & KMOD_SHIFT) {
        handle_shift_and_key_down(key, editor);
        return;
    }

    handle_key_down_no_mod(key, editor);
}

static void handle_key_up(SDL_KeyboardEvent *key, Editor *editor) {
    if(key->keysym.sym == SDLK_LSHIFT || key->keysym.sym == SDLK_RSHIFT) {
        is_shift_down = 0;
        return;
    }
}

void handle_input(SDL_Event *event, Editor *editor, bool *quit) {
    switch(event->type) {
        case SDL_QUIT: { *quit = editor_try_quit(editor); } return;
        case SDL_TEXTINPUT: { handle_textinput(&event->text, editor); } return;
        case SDL_MOUSEBUTTONDOWN: { handle_mouse_button_press(&event->button, editor); } return;
        case SDL_MOUSEBUTTONUP: { handle_mouse_button_release(&event->button, editor); } return;
        case SDL_MOUSEMOTION: { handle_mouse_drag(&event->motion, editor); } return;
        case SDL_MOUSEWHEEL: { handle_mouse_wheel(&event->wheel, editor); } return;
        case SDL_KEYDOWN: { handle_key_down(&event->key, editor); } return;
        case SDL_KEYUP: { handle_key_up(&event->key, editor); } return;
        default: return;
    }
}

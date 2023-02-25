#include "./input.h"
#include "./editor.h"

#define SCROLL_SPEED 30.0f
#define SCROLL_INVERTED -1

static void handle_textinput(SDL_TextInputEvent *text, Editor *editor) {
    editor_insert_text_at_cursor(editor, text->text);
}

static void handle_mouse_button_press(SDL_MouseButtonEvent *button, Editor *editor) {
    if(button->button == SDL_BUTTON_LEFT && button->clicks == 1) {
        // TODO shift click
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
        default: return;
    }
}

static void handle_ctrl_and_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        case SDLK_RIGHT: { editor_skip_word_right(editor); } break;
        case SDLK_LEFT:  { editor_skip_word_left(editor); } break;
        case SDLK_UP:    { editor_swap_lines_up(editor); } break;
        case SDLK_DOWN:  { editor_swap_lines_down(editor); } break;

        case SDLK_o: { editor_load_file(editor); } break;
        case SDLK_s: { editor_save_file(editor); } break;
        case SDLK_n: { editor_new_file(editor); } break;
        case SDLK_c: { editor_try_copy(editor); } break;
        case SDLK_x: { editor_try_cut(editor); } break;
        case SDLK_v: {
            if(SDL_HasClipboardText()) {
                char *clipboard = SDL_GetClipboardText();
                editor_insert_text_at_cursor(editor, clipboard);
                SDL_free(clipboard);
            }
        } break;
    }
}

static void handle_shift_and_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        default: return;
    }
}

static void handle_key_down_no_mod(SDL_KeyboardEvent *key, Editor *editor) {
    switch(key->keysym.sym) {
        case SDLK_RIGHT: { editor_move_cursor_right(editor); } break;
        case SDLK_LEFT:  { editor_move_cursor_left(editor); } break;
        case SDLK_UP:    { editor_move_cursor_up(editor); } break;
        case SDLK_DOWN:  { editor_move_cursor_down(editor); } break;

        case SDLK_BACKSPACE: { editor_delete_char_before_cursor(editor); } break;
        case SDLK_DELETE: { editor_delete_char_after_cursor(editor); } break;
        case SDLK_RETURN: { editor_insert_newline_at_cursor(editor); } break;
        case SDLK_TAB: { editor_insert_text_at_cursor(editor, "\t"); } break;
    }
}

static void handle_key_down(SDL_KeyboardEvent *key, Editor *editor) {
    if((key->keysym.mod & (KMOD_CTRL | KMOD_SHIFT)) == (KMOD_CTRL | KMOD_SHIFT)) {
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

void handle_input(SDL_Event *event, Editor *editor, bool *quit) {
    switch(event->type) {
        case SDL_QUIT: { *quit = editor_try_quit(editor); } return;
        case SDL_TEXTINPUT: { handle_textinput(&event->text, editor); } return;
        case SDL_MOUSEBUTTONDOWN: { handle_mouse_button_press(&event->button, editor); } return;
        case SDL_MOUSEBUTTONUP: { handle_mouse_button_release(&event->button, editor); } return;
        case SDL_MOUSEMOTION: { handle_mouse_drag(&event->motion, editor); } return;
        case SDL_MOUSEWHEEL: { handle_mouse_wheel(&event->wheel, editor); } return;
        case SDL_KEYDOWN: { handle_key_down(&event->key, editor); } return;
    }
}

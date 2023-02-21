#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "./font.h"
#include "./editor.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 728

#define WINDOW_SCALE 2.0

#define STRING_TAB "    "

void scc(int code) {
    if(code < 0) {
        fprintf(stderr, "SDL Error (%d): %s\n", code, SDL_GetError());
        exit(1);
    }
}

void *scp(void *ptr) {
    if(ptr == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

void ftcc(int code, const char *msg) {
    if(code) {
        fprintf(stderr, "FT Error: %s\n", msg);
        exit(1);
    }
}

void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
    (void) source;
    (void) id;
    (void) length;
    (void) userParam;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

/*
char OPEN_FILE_PATH[1024];

void open_file(void) {
    FILE *f = popen("zenity --file-selection", "r");
    fgets(OPEN_FILE_PATH, 1024, f);
    pclose(f);
    printf("Opened file %s\n", OPEN_FILE_PATH);
}
*/

int main(void) {
    // Initialize SDL2
    scc(SDL_Init(SDL_INIT_VIDEO));
    
    SDL_Window *window = scp(SDL_CreateWindow(
        "",
        0, 0,
        WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE,
        SDL_WINDOW_OPENGL// | SDL_WINDOW_RESIZABLE
    ));

    // Initialize OpenGL and Glew
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        int major;
        int minor;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
        printf("GL version %d.%d\n", major, minor);
    }
    scp(SDL_GL_CreateContext(window));
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization error occurred.\n");
        return 1;
    }

    /* if (GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    } else {
        fprintf(stderr, "WARNING: GLEW_ARB_debug_output is not available.\n");
    } */

    // Initialize FreeType
    Font font;
    if(!font_init(&font, "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf")) {
        return 1;
    }

    Renderer renderer;
    if(!renderer_init(&renderer)) {
        return 1;
    }
    renderer_set_resolution(&renderer, WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE);

    Editor editor;
    editor_init(&editor, window, &renderer, &font);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Event loop
    bool quit = false;
    while(!quit) {
        SDL_Event event = {0};
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = editor_try_quit(&editor);
                    break;
                case SDL_TEXTINPUT:
                    editor_insert_text_at_cursor(&editor, event.text.text);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // TODO
                    break;
                case SDL_MOUSEWHEEL:
                    if(event.wheel.x)
                        editor_scroll_x(&editor, event.wheel.preciseX);
                    if(event.wheel.y)
                        editor_scroll_y(&editor, event.wheel.preciseY);
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_o
                    && event.key.keysym.mod & KMOD_CTRL) {
                        editor_load_file(&editor);
                    }
                    else if(event.key.keysym.sym == SDLK_s
                    && event.key.keysym.mod & KMOD_CTRL) {
                        editor_save_file(&editor);
                    }
                    else if(event.key.keysym.sym == SDLK_n
                    && event.key.keysym.mod & KMOD_CTRL) {
                        editor_new_file(&editor);
                    }
                    else switch(event.key.keysym.sym) {
                        case SDLK_RIGHT:
                            editor_move_cursor_right(&editor);
                            break;
                        case SDLK_LEFT:
                            editor_move_cursor_left(&editor);
                            break;
                        case SDLK_UP:
                            editor_move_cursor_up(&editor);
                            break;
                        case SDLK_DOWN:
                            editor_move_cursor_down(&editor);
                            break;
                        case SDLK_BACKSPACE:
                            editor_delete_char_before_cursor(&editor);
                            break;
                        case SDLK_DELETE:
                            editor_delete_char_after_cursor(&editor);
                            break;
                        case SDLK_RETURN:
                            editor_insert_newline_at_cursor(&editor);
                            break;
                        case SDLK_TAB:
                            editor_insert_text_at_cursor(&editor, STRING_TAB);
                            break;
                        }
                    break;
                default:
                    break;
            }
        }

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Vec2f pos = vec2f(0.0f, 1.0f * (float) font.atlas.height);
        //font_render_line(&font, &renderer, hw, hw_len, &pos, vec4f(1.0f, 1.0f, 1.0f, 1.0f));
        editor_render(&editor);

        SDL_GL_SwapWindow(window);
    }

    // Release resources
    editor_destroy(&editor);
    renderer_destroy(&renderer);
    font_destroy(&font);

    SDL_Quit();
    return 0;
}
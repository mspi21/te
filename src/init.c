#include "init.h"

#include <assert.h>
#include <stdio.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 728

#define WINDOW_SCALE 2.0

#define FONT_FILEPATH "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"

static bool scc(int code) {
    if(code < 0)
        fprintf(stderr, "SDL Error (%d): %s\n", code, SDL_GetError());
    return code >= 0;
}

static void *scp(void *ptr) {
    if(!ptr)
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    return ptr;
}

#ifdef GLEW_DEBUG
static void MessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    (void) source;
    (void) id;
    (void) length;
    (void) userParam;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}
#endif

bool init_everything(
    SDL_Window **window_ptr,
    Font *font,
    Renderer *renderer,
    Editor *editor
) {
    // Initialize SDL2
    if(!scc(SDL_Init(SDL_INIT_VIDEO)))
        return false;
    
    if(!(*window_ptr = scp(SDL_CreateWindow(
        "",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    ))))
        return false;

    // Initialize OpenGL and Glew
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    {
        int major;
        int minor;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
        assert(major == 3 && minor == 3);
    }
    if(!scp(SDL_GL_CreateContext(*window_ptr)))
        return false;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization error occurred.\n");
        return false;
    }

#ifdef GLEW_DEBUG
    if (GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    } else {
        fprintf(stderr, "WARNING: GLEW_ARB_debug_output is not available.\n");
    }
#endif

    // Initialize FreeType
    if(!font_init(font, FONT_FILEPATH))
        return false;

    if(!renderer_init(renderer))
        goto fail_free_font;
    renderer_set_resolution(renderer, WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE);

    if(!editor_init(editor, *window_ptr, renderer, font))
        goto fail_free_renderer;

    return true;

fail_free_renderer:
    renderer_destroy(renderer);
fail_free_font:
    font_destroy(font);
    return false;
}

void destroy_everything(Font *font, Renderer *renderer, Editor *editor) {
    editor_destroy(editor);
    renderer_destroy(renderer);
    font_destroy(font);
}
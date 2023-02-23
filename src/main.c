#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./cmd_parser.h"
#include "./input.h"
#include "./init.h"

int main(int argc, char **argv) {
    // Check validity of cmd args
    switch(command_line_check(argc, argv)) {
        case COMMAND_LINE_EXIT_OK: 
            return 0;
        case COMMAND_LINE_EXIT_ERROR:
            return 1;
        default:
            break;
    }

    SDL_Window *window;
    Font font;
    Renderer renderer;
    Editor editor;

    if(!init_everything(&window, &font, &renderer, &editor))
        return 1;

    CommandLineStatus status = command_line_parse(argc, argv, &editor);
    switch(status) {
        case COMMAND_LINE_EXIT_OK:
            goto exit;
        case COMMAND_LINE_EXIT_ERROR:
            goto fail;
        default:
            break;
    }

    // Event loop
    bool quit = false;
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    while(!quit) {
        SDL_Event event = {0};
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                window_w = event.window.data1, window_h = event.window.data2;
                renderer_set_resolution(&renderer, window_w, window_h);
            }
            handle_input(&event, &editor, &quit);
        }

        glViewport(0, 0, window_w, window_h);

        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        editor_render(&editor);

        SDL_GL_SwapWindow(window);
    }

    // Release resources
exit:
    editor_destroy(&editor);
    renderer_destroy(&renderer);
    font_destroy(&font);

    SDL_Quit();
    return 0;

fail:
    editor_destroy(&editor);
    renderer_destroy(&renderer);
    font_destroy(&font);

    SDL_Quit();
    return 1;
}
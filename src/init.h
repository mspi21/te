#ifndef INIT_H_
#define INIT_H_

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "font.h"
#include "renderer.h"
#include "editor.h"

bool init_everything(SDL_Window **window_ptr, Font *font, Renderer *renderer, Editor *editor);

void destroy_everything(Font *font, Renderer *renderer, Editor *editor);

#endif // INIT_H_
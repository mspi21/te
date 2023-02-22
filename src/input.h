#ifndef INPUT_H_
#define INPUT_H_

#include <SDL2/SDL.h>

#include "editor.h"

void handle_input(SDL_Event *event, Editor *editor, bool *quit);

#endif // INPUT_H_
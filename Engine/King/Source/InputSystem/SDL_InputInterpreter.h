#pragma once

#include <SDL.h>

typedef const char* Command;

class SDL_InputInterpreter
{
public:
    SDL_InputInterpreter() {}
    
    Command GetKeyDown();
};
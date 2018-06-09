#include "SDL_InputInterpreter.h"

#include "SDL.h"

Command SDL_InputInterpreter::GetKeyDown()
{
    SDL_Event eve;
    if (SDL_PollEvent(&eve) != 0)
    {
        switch (eve.type)
        {
        case SDL_KEYDOWN:
            switch (eve.key.keysym.sym)
            {
            // TODO Change 1 and 2 to F1 and F2
            case SDLK_1:
                return "1";
            case SDLK_2:
                return "2";
            case SDLK_w:
                return "w";
            case SDLK_a:
                return "a";
            case SDLK_s:
                return "s";
            case SDLK_d:
                return "d";
			case SDLK_h:
				return "h";
			case SDLK_m:
				return "m";
			case SDLK_p:
				return "p";
			case SDLK_q:
				return "q";
			case SDLK_RETURN:
				return "return";
            case SDLK_SPACE:
                return "space";
            case SDLK_ESCAPE:
                return "escape";
            default:
                break;
            }
        default:
            break;
        }
    }
    return "null";
}
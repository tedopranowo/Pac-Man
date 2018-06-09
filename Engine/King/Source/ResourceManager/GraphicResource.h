//------------------------- 
//Created by Shane King 
//Sat 04/08/2017 19:05:04.81 
//------------------------- 
#pragma once 

#include "Resource.h"
#include <SDL.h>
#include <SDL_image.h>

class GraphicResource  : public Resource
{ 
private: 
	/* Private Variables */
    SDL_Surface* m_pGraphic;

public: 
	/* Public Methods */
    GraphicResource(const char* pFilename, SDL_Surface* surface);
    ~GraphicResource() { m_pGraphic = nullptr; }

    SDL_Surface* GetGraphic() const { return m_pGraphic; };

}; 

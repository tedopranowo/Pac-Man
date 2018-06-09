#include "GraphicResource.h" 

//--------------------------------------------------------------- 
// Public Methods 
//---------------------------------------------------------------- 
GraphicResource::GraphicResource(const char* pFilename, SDL_Surface* surface)
    : Resource(pFilename)
    , m_pGraphic(surface)
{
}
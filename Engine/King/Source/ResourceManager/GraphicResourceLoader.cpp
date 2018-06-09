#include "GraphicResourceLoader.h"

#include "ResourceHandle.h"
#include "GraphicResource.h"
#include <SDL.h>
#include <SDL_image.h>
#include <memory>

std::string GraphicResourceLoader::GetPattern()
{
    return "PngFiles";
}

// This will attempt to load the image resource
//		return: Nullptr if it failed to find the image.  Otherwise, the image file resource
std::shared_ptr<ResourceHandle> GraphicResourceLoader::LoadResource(const char* pFilename)
{
    SDL_Surface *pSurface = IMG_Load(pFilename);
    if (pSurface)
    {
        return std::make_shared<ResourceHandle>(std::make_shared<GraphicResource>(pFilename, pSurface));
    }
    else
    {
        return std::shared_ptr<ResourceHandle>();
    }
}

//------------------------- 
//Created by Shane King 
//Wed 04/05/2017  0:44:21.04 
//------------------------- 
#pragma once 

#include "IResourceLoader.h"

class ResourceHandle;

class GraphicResourceLoader : public IResourceLoader
{ 
public: 
    GraphicResourceLoader() {}
    ~GraphicResourceLoader() {}

    virtual std::shared_ptr<ResourceHandle> LoadResource(const char* pFilename) override;
private:

    // Inherited via IResourceLoader
    virtual std::string GetPattern() override;
}; 

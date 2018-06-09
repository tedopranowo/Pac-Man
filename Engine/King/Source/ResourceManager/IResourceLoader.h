//------------------------- 
//Created by Shane King 
//Sat 04/01/2017 14:53:45.42 
//------------------------- 
#pragma once

#include <string>
#include <memory>

class ResourceHandle;
class Resource;

class IResourceLoader 
{ 
public: 
	/* Public Methods */ 
    virtual std::string GetPattern() = 0;
    virtual std::shared_ptr<ResourceHandle> LoadResource(const char* pFilename) = 0;

}; 

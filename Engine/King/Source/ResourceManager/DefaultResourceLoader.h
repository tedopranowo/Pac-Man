//------------------------- 
//Created by Shane King 
//Sat 04/01/2017 14:59:45.64 
//------------------------- 
#pragma once 

#include "IResourceLoader.h"

class DefaultResourceLoader : public IResourceLoader
{ 

public: 
    DefaultResourceLoader() {}
    ~DefaultResourceLoader() {}

    virtual std::string GetPattern() override { return "*"; }
}; 

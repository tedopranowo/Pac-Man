//------------------------- 
//Created by Shane King 
//Sat 05/06/2017  0:17:28.58 
//------------------------- 
#pragma once

#include "IResourceLoader.h"

class ActorFactory;

class ActorResourceLoader : public IResourceLoader
{
private:
    ActorFactory* m_pActorFactory;

public: 
	ActorResourceLoader(ActorFactory* pActorFactory);
    ~ActorResourceLoader() { m_pActorFactory = nullptr; }

private: 
    virtual std::string GetPattern() override;
    virtual std::shared_ptr<ResourceHandle> LoadResource(const char * pFilename) override;
}; 

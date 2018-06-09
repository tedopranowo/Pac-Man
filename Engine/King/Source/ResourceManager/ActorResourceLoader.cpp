#include "ActorResourceLoader.h" 

#include "../Actors/ActorFactory.h"
#include "ActorResource.h"
#include "ResourceHandle.h"

ActorResourceLoader::ActorResourceLoader(ActorFactory* pActorFactory)
    : m_pActorFactory(pActorFactory)
{
}

std::string ActorResourceLoader::GetPattern()
{
    return "ActorData";
}

// This will attempt to load the actor's resource file
// 		return: nullptr, if it failed to find the resource.  Otherwise, returns the actor resource
std::shared_ptr<ResourceHandle> ActorResourceLoader::LoadResource(const char * pFilename)
{
    StrongActorPtr actor = m_pActorFactory->CreateActor(pFilename);
    if (actor)
    {
        return std::make_shared<ResourceHandle>(std::make_shared<ActorResource>(pFilename, actor));
    }
    else
    {
        return std::shared_ptr<ResourceHandle>();
    }
}

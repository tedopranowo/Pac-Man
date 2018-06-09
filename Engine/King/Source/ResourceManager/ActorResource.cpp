#include "ActorResource.h" 

ActorResource::ActorResource(const char* pFilename, StrongActorPtr pActor)
    : Resource(pFilename)
    , m_pActor(pActor)
{
}

ActorResource::~ActorResource()
{
    m_pActor->Destroy();
}
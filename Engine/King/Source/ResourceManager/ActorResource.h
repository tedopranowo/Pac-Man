//------------------------- 
//Created by Shane King 
//Sat 05/06/2017  0:54:49.09 
//------------------------- 
#pragma once 

#include "Resource.h"
#include "../Actors//Actor.h"
#include "../Actors/TypedefActors.h"

class ActorResource : public Resource
{ 
private:
    StrongActorPtr m_pActor;

public:
	ActorResource(const char* pFilename, StrongActorPtr pActor);
    ~ActorResource();

    std::shared_ptr<Actor> GetActor() { return m_pActor; }
}; 
